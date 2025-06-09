#ifndef __string_c__
#define __string_c__

#include "memory.h"
#include "string.h"

int stringToInt(LPTSTR s) {
    int num = 0;
    int i = 0;
    BOOL isNegative = FALSE;
    if (s[i] == '-') {
        isNegative = TRUE;
        i ++;
    }
    while (s[i] && (s[i] >= '0' && s[i] <= '9')) {
        num = num * 10 + (s[i] - '0');
        i ++;
    }
    return isNegative ? -num : num;
}

LPTSTR stringDuplicate(const LPTSTR s) {

    DWORD length = stringLength(s);

    LPTSTR result = (LPTSTR) memoryAlloc((length + 1) * sizeof(TCHAR));
    memoryCopy(result, s, length * sizeof(TCHAR));

    return result;
}

LPTSTR stringConcat(const LPTSTR s1, const LPTSTR s2) {

    DWORD s1Length = stringLength(s1);
    DWORD s2Length = stringLength(s2);

    LPTSTR result = (LPTSTR) memoryAlloc((s1Length + s2Length + 1) * sizeof(TCHAR));
    memoryCopy(result, s1, s1Length * sizeof(TCHAR));
    memoryCopy(&result[s1Length], s2, s2Length * sizeof(TCHAR));
    result[s1Length + s2Length] = '\0';

    return result;
}

LPWSTR stringMultiByteToWide(const LPSTR lpMultiByteStr) {

    int length = strlenMultiByte(lpMultiByteStr);
    if (!SUCCEEDED(length))
        return NULL;

    LPWSTR lpWideCharStr = (LPWSTR) memoryAlloc((length + 1) * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;

    length = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpMultiByteStr, -1, lpWideCharStr, length);
    if (!SUCCEEDED(length)) {
        memoryFree(lpWideCharStr);
        return NULL;
    }

    return lpWideCharStr;
}

#endif /* __string_c__ */
