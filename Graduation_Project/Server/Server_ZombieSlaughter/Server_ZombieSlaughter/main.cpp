#include "stdafx.h"
#include "Protocol.h"

unordered_map<int, CLIENT> clients;


void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);


void Update(int);
int get_new_id()
{
	for (int i = 0; i <= MAX_USER; ++i)
	{
		if (clients.count(i) == 0)return i;
	}
}

void recvData(int c_id)
{

	//리시브 버퍼의 위치= 받는 리시브의 데이터위치+남은데이터의크기
	clients[c_id].m_recv_over.wsaBuf[0].buf = reinterpret_cast<char*>(clients[c_id].m_recv_over.databuffer) + clients[c_id].m_prev_size;

	//받는 데이터의 크기=전체버퍼크기-남은 데이터크기
	clients[c_id].m_recv_over.wsaBuf[0].len = MAX_BUFFER - clients[c_id].m_prev_size;

	memset(&clients[c_id].m_recv_over.over, 0, sizeof(clients[c_id].m_recv_over.over));
	DWORD r_flag = 0;
	WSARecv(clients[c_id].m_socket, clients[c_id].m_recv_over.wsaBuf, 1, NULL, &r_flag, &clients[c_id].m_recv_over.over, recv_callback);


}
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

		int c_id = get_new_id();
		clients[c_id] = CLIENT{};
		clients[c_id].m_id = c_id;
		clients[c_id].m_socket = c_sock;
		clients[c_id].m_prev_size = 0;
		clients[c_id].m_recv_over.m_id = c_id;

		while (1) {

		recvData(c_id);
		}

	}
	closesocket(listenSocket);
	WSACleanup();



}

void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	//socketInfo의 주소가 overlapped의 주소와 같다 왜냐하면 구조체의 첫번째가 overlapped이기 때문이다.
	int c_id = reinterpret_cast<SOCKETINFO*>(overlapped)->m_id;

	if (dataBytes == 0)
	{
		closesocket(clients[c_id].m_socket);
		clients.erase(c_id);
		return;
	}


	Update(c_id);
	memset(&(clients[c_id].m_recv_over), 0, sizeof(WSAOVERLAPPED));

	s2c_move packet;
	packet.size = sizeof(s2c_move);
	packet.type = S2C_MOVE;
	packet.x = clients[c_id].x;
	packet.y = clients[c_id].y;
	packet.z = clients[c_id].z;
	SOCKETINFO send_Info = {};
	send_Info.wsaBuf[0].buf = reinterpret_cast<char*>(&packet);
	send_Info.wsaBuf[0].len = packet.size;
	memset(&send_Info.over, 0, sizeof(WSAOVERLAPPED));
	DWORD s_flag = 0;
	WSASend(clients[c_id].m_socket, send_Info.wsaBuf, 1, NULL, 0, &send_Info.over, send_callback);

}
void Update(int c_id)
{
	switch (clients[c_id].m_recv_over.databuffer[1])
	{
	case C2S_KEY_EVENT://클라에서 키이벤트가 들어왔을때
	{
		c2s_Key* keyEvent = reinterpret_cast<c2s_Key*>(clients[c_id].m_recv_over.databuffer);
		switch (keyEvent->key)
		{
		case 'w':
		case 'W':
			clients[c_id].z += 100.0f;
			break;

		case 'a':
		case 'A':
			clients[c_id].x -= 100.0f;
			break;
		case 's':
		case 'S':
			clients[c_id].z += 100.0f;
			break;
		case 'd':
		case 'D':
			clients[c_id].x += 100.0f;
			
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
	int c_id = reinterpret_cast<SOCKETINFO*>(overlapped)->m_id;
	recvData(c_id);
}
