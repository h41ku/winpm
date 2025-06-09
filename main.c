#ifndef __main_h__
#define __main_h__

#include "winapi.h"
#include "string.h"
#include "service.h"
#include "winpm.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (argv == NULL) {
        showMessage(__T("Couldn't parse arguments."));
        return 1;
    }

    if (argc > 1) {
        if (stringCompare(argv[1], __T("install")) == 0) {
            requestAdminPrivileges(argv[0], argv[1]);
            installService(argv[0], NULL, NULL);
        } else if (stringCompare(argv[1], __T("uninstall")) == 0) {
            requestAdminPrivileges(argv[0], argv[1]);
            uninstallService();
        } else if (stringCompare(argv[1], __T("run")) == 0) {
            runProcesses();
        } else if (stringCompare(argv[1], __T("run-service")) == 0) {
            runService();
        } else {
            usage();
        }
    } else {
        usage();
    }

    LocalFree(argv);

    return 0;
}

#endif /* __main_h__ */
