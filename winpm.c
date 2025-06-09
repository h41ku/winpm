#ifndef __winpm_c__
#define __winpm_c__

#include "winapi.h"
#include "winpm.h"
#include "memory.h"
#include "string.h"

LPTSTR caption = __T("WinPM");
Node *processesList = NULL;

const LPTSTR getCaption() {

    return caption;
}

DWORD WINAPI autostartProcess(LPVOID lpParameter) {

    Process *process = (Process *) lpParameter;

    Sleep(process->delay);

    if (CreateProcess(
        NULL,
        process->command,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
        NULL,
        process->cwd,
        &process->startupInfo,
        &process->processInfo
    )) {

        WaitForSingleObject(process->processInfo.hProcess, INFINITE);

        CloseHandle(process->processInfo.hProcess);
        CloseHandle(process->processInfo.hThread);
    }

    process->processInfo.hProcess = INVALID_HANDLE_VALUE;
    process->processInfo.hThread = INVALID_HANDLE_VALUE;

    return 0;
}

DWORD WINAPI respawnProcess(LPVOID lpParameter) {

    Process *process = (Process *) lpParameter;

    while (TRUE) {

        if (CreateProcess(
            NULL,
            process->command,
            NULL,
            NULL,
            FALSE,
            NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
            NULL,
            process->cwd,
            &process->startupInfo,
            &process->processInfo
        )) {

            WaitForSingleObject(process->processInfo.hProcess, INFINITE);

            CloseHandle(process->processInfo.hProcess);
            CloseHandle(process->processInfo.hThread);
        }

        process->processInfo.hProcess = INVALID_HANDLE_VALUE;
        process->processInfo.hThread = INVALID_HANDLE_VALUE;

        Sleep(process->delay);
    }

    return 0;
}

int escapeCommandLineArgument(LPTSTR dst, LPWSTR arg) {

    int i = 0;
    int j = 0;
    TCHAR c;

    dst[j ++] = '"';
    while ((c = arg[i ++]) != '\0') {
        if (c == '"' || c == '\\') {
            dst[j ++] = '\\';
        }
        dst[j ++] = c;
    }
    dst[j ++] = '"';
    dst[j] = '\0';

    return j;
}

void parseProcessSettings(Process *process) {

    LPTSTR command = NULL;

    process->cwd = NULL;
    process->processInfo.hProcess = INVALID_HANDLE_VALUE;
    process->hThread = INVALID_HANDLE_VALUE;
    process->mode = WINPM_MODE_AUTOSTART;
    process->delay = 3000;
    process->isInvalidSettings = TRUE;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(process->command, &argc);
    if (argv == NULL) {
        return;
    }

    int i;
    for (i = 0; i < argc; i ++) {
        LPWSTR arg = argv[i];
        if (stringCompare(arg, __T("respawn")) == 0) {
            process->mode = WINPM_MODE_RESPAWN;
        } else if (stringCompare(arg, __T("autostart")) == 0) {
            process->mode = WINPM_MODE_AUTOSTART;
        } else if (stringCompare(arg, __T("delay")) == 0) {
            if (i + 1 >= argc)
                break;
            process->delay = stringToInt(argv[++ i]);
        } else if (stringCompare(arg, __T("cwd")) == 0) {
            if (i + 1 >= argc)
                break;
            process->cwd = stringDuplicate(argv[++ i]);
        } else if (stringCompare(arg, __T("--")) == 0) {
            if (++ i < argc) {
                int j;
                int length = 0;
                for (j = i; j < argc; j ++)
                    length += stringLength(argv[j]) * 2 + 2 + 1;
                if (length > 0) {
                    command = (LPTSTR) memoryAlloc(length * sizeof(TCHAR));
                    length = 0;
                    for (j = i; j < argc; j ++) {
                        length += escapeCommandLineArgument(&command[length], argv[j]);
                        command[length ++] = ' ';
                    }
                    command[length - 1] = '\0';
                }
            }
            break;
        }
    }

    LocalFree(argv);

    if (command) {
        memoryFree(process->command);
        process->command = command;
        process->isInvalidSettings = FALSE;
    }
}

void addProcess(const LPSTR command) {

    Process *process = (Process *) memoryAlloc(sizeof(Process));
    process->command = stringMultiByteToWide(command);

    parseProcessSettings(process);

    if (!process->isInvalidSettings) {
        process->hThread = CreateThread(
            NULL,
            0,
            process->mode == WINPM_MODE_AUTOSTART
                ? &autostartProcess
                : &respawnProcess,
            process,
            0,
            NULL
        );
    }

    Node *node = (Node *) memoryAlloc(sizeof(Node));
    node->data = process;

    node->next = processesList ? processesList : NULL;
    processesList = node;
}

void removeAllProcesses() {

    Node *node = processesList;

    while (node) {

        Node *next = node->next;

        Process *process = (Process *) node->data;

        if (process->hThread != INVALID_HANDLE_VALUE) {
            TerminateThread(process->hThread, 0);
            CloseHandle(process->hThread);
        }

        if (process->processInfo.hProcess != INVALID_HANDLE_VALUE) {
            TerminateProcess(process->processInfo.hProcess, 0);
            CloseHandle(process->processInfo.hProcess);
            CloseHandle(process->processInfo.hThread);
            process->processInfo.hProcess = INVALID_HANDLE_VALUE;
            process->processInfo.hThread = INVALID_HANDLE_VALUE;
        }

        memoryFree(process->command);
        memoryFree(process);
        memoryFree(node);

        node = next;
    }
}

void readConfigFile() {

    TCHAR configFileName[MAX_PATH];
    DWORD length = GetModuleFileName(NULL, configFileName, MAX_PATH);
    {
        int i = length;
        configFileName[i - 3] = 'c';
        configFileName[i - 2] = 'n';
        configFileName[i - 1] = 'f';
    }

    HANDLE hConf = CreateFile(configFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hConf == INVALID_HANDLE_VALUE) {
        showMessage(_T("Can't read configuration file."));
        return;
    }

    {
        LPSTR buffer = (LPSTR) memoryAlloc(bufferSize);
        int lineCapacity = initialLineCapacity;
        int lineLength = 0;
        LPSTR line = (LPSTR) memoryAlloc(lineCapacity);
        BOOL isComment = FALSE;

        while (ReadFile(hConf, buffer, bufferSize, &length, NULL) && length > 0) {
            for (int i = 0; i < length; i ++) {
                char c = buffer[i];
                if (c == '\r') {
                    continue;
                }
                if (c == '\n') {
                    if (isComment) {
                        isComment = FALSE;
                    }
                    line[lineLength] = '\0';
                    if (lineLength > 0) {
                        addProcess(line);
                        lineLength = 0;
                    }
                    continue;
                }
                if (!isComment) {
                    if (c == '#') {
                        isComment = TRUE;
                        continue;
                    }
                    line[lineLength ++] = c;
                    if (lineLength == lineCapacity) {
                        int newLineCapacity = lineCapacity + initialLineCapacity;
                        line = memoryRealloc(line, lineCapacity, newLineCapacity);
                        lineCapacity = newLineCapacity;
                    }
                }
            }
        }

        if (lineLength > 0) {
            addProcess(line);
            lineLength = 0;
        }

        memoryFree(line);
        memoryFree(buffer);
    }

    CloseHandle(hConf);
}

void usage() {
    
    showMessage(__T(
        "usage:\n"
        "  help - show this message\n"
        "  install - install service\n"
        "  install <user> <passwd> - same as install but run as given user\n"
        "  uninstall - uninstall service\n"
        "  run - run processes from configuration file\n"
    ));
}

void runProcesses() {

    MSG msg;

    readConfigFile();

    while (GetMessage(&msg, NULL, 0, 0)) { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 

    removeAllProcesses();
    ExitProcess(0);
}

BOOL hasAdminPrivileges() {

    BOOL result;
    SC_HANDLE hSCM = NULL;

    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCM != NULL) {
        result = TRUE;
        CloseServiceHandle(hSCM);
    } else {
        result = FALSE;
    }
    
    return result;
}

void requestAdminPrivileges(LPTSTR appPath, LPTSTR arguments) {

    if (!hasAdminPrivileges()) {
        ShellExecute(NULL, __T("runas"), appPath, arguments, NULL, SW_SHOWNORMAL);
        ExitProcess(0);
    }
}

#endif /* __winpm_c__ */
