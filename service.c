#ifndef __service_c__
#define __service_c__

#include "service.h"
#include "string.h"
#include "memory.h"
#include "winpm.h"

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

void stopService() {

    removeAllProcesses();
}

void serviceCtrlHandler(DWORD opcode) {

    if (opcode == SERVICE_CONTROL_STOP) {

        stopService();

        serviceStatus.dwWin32ExitCode = 0;
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwCheckPoint = 0;
        serviceStatus.dwWaitHint = 0;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

    } else {

        SetServiceStatus(serviceStatusHandle, &serviceStatus);
    }
}

void startService(DWORD argc, LPTSTR argv[]) {

    DWORD status = NO_ERROR;
    DWORD specificError = 0;

    serviceStatus.dwServiceType = SERVICE_WIN32;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    serviceStatusHandle = RegisterServiceCtrlHandler(getCaption(), serviceCtrlHandler);
    if (serviceStatusHandle == (SERVICE_STATUS_HANDLE) 0) {
        status = GetLastError();
    }
 
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    if (status != NO_ERROR) {
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = status;
        serviceStatus.dwServiceSpecificExitCode = specificError;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        return;
    }
 
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus)) {
        return;
    }

    runProcesses();
}

void runService() {

    SERVICE_TABLE_ENTRY dispatchTable[] = {
        { getCaption(), startService },
        { NULL, NULL }
    };

    StartServiceCtrlDispatcher(dispatchTable);
}

void installService(const LPTSTR path, const LPTSTR userName, const LPTSTR password) {

    SC_HANDLE hService = NULL;
    SC_HANDLE hSCM = NULL;
    BOOL result = FALSE;

    LPTSTR runServiceCommand = stringConcat(path, __T(" run-service"));

    if ((hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)) != NULL) {

        hService = CreateService(hSCM, getCaption(), 
            NULL, SERVICE_START, SERVICE_WIN32_OWN_PROCESS, 
            SERVICE_AUTO_START,
            SERVICE_ERROR_IGNORE, 
            runServiceCommand, NULL, NULL, NULL,
            userName,
            password
        );

        if (hService != NULL) {
            if (!StartService(hService, 0, NULL)) {
                showMessage(__T("Couldn't start service."));
            } else {
                result = TRUE;
            }
            CloseServiceHandle(hService);
        } else {
            showMessage(__T("Couldn't create service."));
        }

        CloseServiceHandle(hSCM);

    } else {

        showMessage(__T("Couldn't connect to service control manager."));
    }

    memoryFree(runServiceCommand);

    if (result) {
        showMessage(__T("Service successfully installed."));
    }
}

void uninstallService() {

    SERVICE_STATUS status;
    SC_HANDLE hService;
    SC_HANDLE hSCM;
    BOOL result = FALSE;

    if ((hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT)) != NULL) {

        if ((hService = OpenService(hSCM, getCaption(), SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE)) == NULL) {

            showMessage(__T("Couldn't open service."));
            CloseServiceHandle(hSCM);

        } else {

            if (QueryServiceStatus(hService, &status)) {
                BOOL tryToDeleteService = TRUE;
                if (status.dwCurrentState != SERVICE_STOPPED) {
                    if (ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
                        if (status.dwCurrentState != SERVICE_STOPPED) {
                            showMessage(__T("Service doesn't stopped."));
                            tryToDeleteService = FALSE;
                        }
                    } else {
                        showMessage(__T("Couldn't stop service."));
                        tryToDeleteService = FALSE;
                    }
                }
                if (tryToDeleteService) {
                    if (DeleteService(hService)) {
                        result = TRUE;
                    } else {
                        showMessage(__T("Couldn't delete service."));
                    }
                }
            } else {
                showMessage(__T("Couldn't query status of service."));
            }

            CloseServiceHandle(hService);
            CloseServiceHandle(hSCM);
        }

    } else {

        showMessage(__T("Couldn't connect to service control manager."));
    }

    if (result) {
        showMessage(__T("Service successfully uninstalled."));
    }
}

#endif /* __service_c__ */
