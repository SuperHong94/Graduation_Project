#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx를위해서
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx를위해서


#include <iostream>
#include <unordered_map>

#define MAX_BUFFER        1024
#define SERVER_PORT        3500
constexpr int MAX_USER = 10;
using namespace std;

struct SOCKETINFO
{
	WSAOVERLAPPED over;
	WSABUF wsaBuf[1];
	unsigned char databuffer[MAX_BUFFER];
};

struct CLIENT
{
	SOCKET m_socket;
	int m_id;

	SOCKETINFO m_recv_over;
	unsigned int m_prev_size;

	float x, y, z;
};

void err_display(char* msg);