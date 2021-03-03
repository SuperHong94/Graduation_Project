#pragma once
#pragma comment(lib, "ws2_32")
#include <WS2tcpip.h>  

//#include <ws2tcpip.h>
#include <iostream>
#include <map>
#include <algorithm>
using namespace std;


#define SERVERPORT 9000
#define BUFSIZE    512

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[BUFSIZE];
};



// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);

// 소켓 함수 오류 출력
void err_display(char* msg);