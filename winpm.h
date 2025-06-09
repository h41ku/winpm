#ifndef __winpm_h__
#define __winpm_h__

#include "winapi.h"

#define bufferSize 16*1024
#define initialLineCapacity 16

typedef struct _Node {
    void *data;
    struct _Node *next;
} Node;

#define WINPM_MODE_AUTOSTART 1
#define WINPM_MODE_RESPAWN 2

typedef struct _Process {
    LPTSTR cwd;
    LPTSTR command;
    int mode;
    int delay;
    BOOL isInvalidSettings;
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    HANDLE hThread;
} Process;

const LPTSTR getCaption();
DWORD WINAPI autostartProcess(LPVOID lpParameter);
DWORD WINAPI respawnProcess(LPVOID lpParameter);
void addProcess(const LPSTR command);
void removeAllProcesses();
void readConfigFile();
void usage();
void runProcesses();
void requestAdminPrivileges(LPTSTR appPath, LPTSTR arguments);

#define showMessage(message) MessageBox(NULL, message, getCaption(), MB_OK)

#endif /* __winpm_h__ */
