#include "stdafx.h"



// ���� �Լ� ���� ��� �� ����
//void err_quit(char* msg)
//{
//    LPVOID lpMsgBuf;
//    FormatMessage(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//        NULL, WSAGetLastError(),
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//        (LPTSTR)&lpMsgBuf, 0, NULL);
//    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
//    LocalFree(lpMsgBuf);
//    exit(1);
//}

void display_error(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

// ���� �Լ� ���� ���
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