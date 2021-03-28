#include "stdafx.h"
#include "Protocol.h"

unordered_map<SOCKET, SOCKETINFO> clients;


void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);


void Update(SOCKET);

int main()
{

	int ret = 0;
	WSADATA WSAdata;
	WSAStartup(MAKEWORD(2, 2), &WSAdata);

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == SOCKET_ERROR)
	{
		err_display("WSASocket()");
		return 0;
	}
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


	ret = bind(listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		err_display("bind()");
		return 0;
	}


	ret = listen(listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		err_display("listen()");
		return 0;
	}


	SOCKET c_sock; //클라이언트 소켓
	SOCKADDR_IN c_addr;
	int addrlen = sizeof(c_addr);


	while (1)
	{
		c_sock = accept(listenSocket, reinterpret_cast<SOCKADDR*>(&c_addr), &addrlen);
		if (c_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
		clients[c_sock]= SOCKETINFO{};
		clients[c_sock].socket = c_sock;
		clients[c_sock].wsaBuf.len = MAX_BUFFER;
		clients[c_sock].wsaBuf.buf = clients[c_sock].databuffer;

		memset(&clients[c_sock].over, 0, sizeof(WSAOVERLAPPED));

		DWORD flags = 0;
		WSARecv(clients[c_sock].socket, &clients[c_sock].wsaBuf, 1, NULL, &flags, &(clients[c_sock].over), recv_callback);

	}
	closesocket(listenSocket);
	WSACleanup();



}

void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	//socketInfo의 주소가 overlapped의 주소와 같다 왜냐하면 구조체의 첫번째가 overlapped이기 때문이다.
	SOCKET clients_s = reinterpret_cast<SOCKETINFO*>(overlapped)->socket;

	if (dataBytes == 0)
	{
		closesocket(clients[clients_s].socket);
		clients.erase(clients_s);
		return;
	}
	
	
	Update(clients_s);
	memset(&(clients[clients_s].over), 0, sizeof(WSAOVERLAPPED));

}
void Update(SOCKET c_sock)
{
	switch (clients[c_sock].databuffer[1])
	{
	case C2S_KEY_EVENT://클라에서 키이벤트가 들어왔을때
	{
		c2s_Key* keyEvent = reinterpret_cast<c2s_Key*>(clients[c_sock].databuffer);
		switch (keyEvent->key)
		{
		case 'w':
		case 'W':
			break;

		case 'a':
		case 'A':
			break;
		case 's':
		case 'S':
			break;
		case 'd':
		case 'D':
			break;
		default:
			break;
		}
	}

	default:
		break;
	}
	
}
void send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
}
