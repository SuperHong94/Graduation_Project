#include "stdafx.h"


void err_display(char* msg)
{
    WCHAR* lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    cout << msg;
    wcout << lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
}