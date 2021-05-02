#include "stdafx.h"
#include "Protocol.h"


struct SOCKETINFO
{
	WSAOVERLAPPED m_overlapped;
	WSABUF m_wsabuf[1];
	unsigned char m_buf[MAX_BUFFER];
	int m_id;
	//DWORD m_dataBytes;
};

struct SESSION
{
	SOCKET m_socket;
	int m_id;
	SOCKETINFO m_recvOver;
	unsigned int m_prev_size;
	short x, y;
};


unordered_map<int, SESSION> clients;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
//void do_recv(int s_id);
void proccess_packet(int p_id, unsigned char* p_buf);



void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKETINFO* socketInfo = reinterpret_cast<SOCKETINFO*>(overlapped);
	if (dataBytes == 0) {
		closesocket(clients[socketInfo->m_id].m_socket);
		clients.erase(socketInfo->m_id);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	delete socketInfo;
}

int GetNewID()
{
	for (int i = 0; i < MAX_USER; ++i)
		if (clients.count(i) == 0)return i;
	return -1;
}

void do_recv(int s_id)
{


	clients[s_id].m_recvOver.m_wsabuf->buf = reinterpret_cast<char*>(clients[s_id].m_recvOver.m_buf) + clients[s_id].m_prev_size;

	clients[s_id].m_recvOver.m_wsabuf->len = MAX_BUFFER - clients[s_id].m_prev_size;
	memset(&clients[s_id].m_recvOver.m_overlapped, 0, sizeof(clients[s_id].m_recvOver.m_overlapped));

	DWORD r_flag = 0;
	WSARecv(clients[s_id].m_socket, clients[s_id].m_recvOver.m_wsabuf, 1, NULL, &r_flag, &clients[s_id].m_recvOver.m_overlapped, recv_callback);



}


void send_packet(int p_id, void* p)
{
	int p_size = reinterpret_cast<unsigned char*>(p)[0];
	int p_type = reinterpret_cast<unsigned char*>(p)[1];

//#ifdef _DEBUG
//
//	cout << "To client [ " << p_id << "]: ";
//	cout << "Packet [ " << p_type << "]\n";
//#endif // DEBUG




	SOCKETINFO* s_over = new SOCKETINFO;
	s_over->m_id = p_id;
	memset(&s_over->m_overlapped, 0, sizeof(s_over->m_overlapped));
	memcpy(s_over->m_buf, p, p_size);
	s_over->m_wsabuf->buf = reinterpret_cast<char*>(s_over->m_buf);
	s_over->m_wsabuf->len = p_size;

	WSASend(clients[p_id].m_socket, s_over->m_wsabuf, 1, NULL, 0, &s_over->m_overlapped, send_callback);
}
void send_login_ok_packet(int p_id)
{
	s2c_login_ok p;

	p.id = p_id;

	p.size = sizeof(s2c_login_ok);
	p.type = S2C_LOGIN_OK;
	p.x = clients[p_id].x;
	p.y = clients[p_id].y;
	send_packet(p_id, &p);
}

void send_move_packet(int p_id)
{
	s2c_move_player p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_MOVE_PLAYER;
	p.x = clients[p_id].x;
	p.y = clients[p_id].y;

	for (auto i = clients.begin(); i != clients.end(); ++i)
		send_packet(i->first, &p);
}
void do_move(int p_id, DIRECTION dir)
{
	int offX = 28;
	int offY = 28;
	auto& x = clients[p_id].x;
	auto& y = clients[p_id].y;
	switch (dir)
	{
	case D_N:
		if (y - offY > -20)y = y - offY;
		break;
	case D_S:
		if (y + offY < 200) y = y + offY;
		break;
	case D_E:
		if (x + offX < 200)
			x = x + offX;
		break;
	case D_W:
		if (x - offX > -20)
			x = x - offX;
		break;
	default:
		break;
	}
	send_move_packet(p_id);
}
void send_add_packet(int p_id, int add_id)
{
	s2c_add_player packet;
	packet.size = sizeof(packet);
	packet.id = add_id;
	packet.x = clients[add_id].x;
	packet.y = clients[add_id].y;
	packet.type = S2C_ADD_PLAYER;
	send_packet(p_id, &packet);
}
void proccess_packet(int p_id, unsigned char* p_buf)
{
	switch (p_buf[1])
	{
	case C2S_LOGIN:
	{
		c2s_login* packet = reinterpret_cast<c2s_login*>(p_buf);

		send_login_ok_packet(p_id);

		//새로 추가된 클라이언트에게 기존의 클라이언트정보 보내기
		for (auto i = clients.begin(); i != clients.end(); ++i)
			send_add_packet(p_id, i->first);

		//이미접속한 클라이언트에게 추가된 클라이언트정보 보내기
		for (auto i = clients.begin(); i != clients.end(); ++i)
			send_add_packet(i->first, p_id);

	}
	break;
	case C2S_MOVE:
	{
		c2s_move* packet = reinterpret_cast<c2s_move*>(p_buf);
		do_move(p_id, packet->dr);
	}
	break;
	default:
		cout << "Unknown Packet Type form Client[" << p_id << "] Packet Type[" << p_buf[1] << "]" << endl;
		while (true); //디버깅을위해 멈춰이쓴다.
	}
}
int main()
{
	wcout.imbue(std::locale("korean"));
	int retval = 0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET) err_display("socket()");



	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);

	// bind() n
	retval = bind(listen_sock, reinterpret_cast<SOCKADDR*>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_display("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_display("listen()");
	// 데이터 통신에 사용할 변수


	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);


	while (true)
	{
		SOCKET c_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (c_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}


		// 접속한 클라이언트 정보 출력
		/*printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/


		//Accept 처리
		int c_id = GetNewID();
		if (c_id != -1) {
			clients[c_id] = SESSION{};
			clients[c_id].m_socket = c_sock;
			clients[c_id].m_id = c_id;
			clients[c_id].m_recvOver.m_id = c_id;

			clients[c_id].m_prev_size = 0;
			clients[c_id].x = 0;
			clients[c_id].y = 0;


			do_recv(c_id);

		}
		else {

			//#ifdef _DEBUG
			//	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			//#endif // _DEBUG
			closesocket(c_sock);
		}


	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

}
void send_remove_packet(int p_id)
{

	s2c_remove_player p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = S2C_REMOVE_PLAYER;

	for (auto i = clients.begin(); i != clients.end(); ++i) {
		if (i->first != p_id)
			send_packet(i->first, &p);
	}
}
void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKETINFO* socketInfo = reinterpret_cast<SOCKETINFO*>(overlapped);
	if (dataBytes == 0) {
//#ifdef _DEBUG
//		cout << "접속종료" << endl;
//#endif // _DEBUG

		send_remove_packet(socketInfo->m_id);
		closesocket(clients[socketInfo->m_id].m_socket);
		clients.erase(socketInfo->m_id);
		return;
	}

	unsigned char* packet_ptr = socketInfo->m_buf;
	int key = socketInfo->m_id;
	int num_data = dataBytes + clients[key].m_prev_size; //처리해야할 데이터의 크기=지금받은데이터+지난번에 받은데이터의 나머지

	int packet_size = packet_ptr[0]; //지금 처리해야하는 패킷의 크기

	while (num_data >= packet_size) {
		proccess_packet(key, packet_ptr);
		num_data -= packet_size;
		packet_ptr += packet_size;
		if (0 >= num_data)break;//이조건을 줘야됨
		packet_size = packet_ptr[0];
	}
	clients[key].m_prev_size = num_data;//남은 찌꺼기 보존
	if (0 != num_data)
		memcpy(socketInfo->m_buf, packet_ptr, sizeof(num_data));//맨앞으로 복사

	do_recv(key);

}