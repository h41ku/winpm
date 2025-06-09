#ifndef __string_h__
#define __string_h__

#include "winapi.h"

#define stringLength(s) lstrlen(s)
#define stringCompare(s1, s2) lstrcmp(s1, s2)
LPTSTR stringDuplicate(const LPTSTR s);
LPTSTR stringConcat(const LPTSTR s1, const LPTSTR s2);
int stringToInt(const LPTSTR s);

#define strlenMultiByte(lpMultiByteStr) MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpMultiByteStr, -1, NULL, 0)
LPWSTR stringMultiByteToWide(const LPSTR lpMultiByteStr);

#endif /* __string_h__ */
