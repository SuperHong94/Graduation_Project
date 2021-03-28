#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx를위해서
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx를위해서


#include <iostream>
#include <unordered_map>

#define MAX_BUFFER        1024
#define SERVER_PORT        3500
using namespace std;

struct SOCKETINFO
{
	WSAOVERLAPPED over;
	WSABUF wsaBuf;
	SOCKET socket;
	char databuffer[MAX_BUFFER];
};

void err_display(char* msg);