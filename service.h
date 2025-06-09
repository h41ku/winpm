#ifndef __service_h__
#define __service_h__

#include "winapi.h"

void startService(DWORD argc, LPTSTR argv[]);
void stopService();

void serviceCtrlHandler(DWORD opcode);
void runService();

void installService(const LPTSTR path, const LPTSTR userName, const LPTSTR password);
void uninstallService();

#endif /* __service_h__ */
