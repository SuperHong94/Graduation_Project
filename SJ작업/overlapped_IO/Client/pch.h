﻿// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가

#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")S
#include <Windows.h>
#include "framework.h"
#include <iostream>
#endif //PCH_H

#define WINWIDTH 275
#define WINHEIGHT 295
#define OFFSETX	7
#define OFFSETY	7
#define BUFSIZE 512

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#pragma pack(push,1)
struct Data
{
	//char type;
	short x = 0;
	short y = 0;
};
#pragma pack(pop)




// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);

// 소켓 함수 오류 출력
void err_display(char* msg);