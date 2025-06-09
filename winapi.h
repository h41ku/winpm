#ifndef __winapi_h__
#define __winapi_h__

#ifndef UNICODE
#define UNICODE
#endif

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#endif
#if defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <shellapi.h>

#endif /* __winapi_h__ */
