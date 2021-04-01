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
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���

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

		//���� �߰��� Ŭ���̾�Ʈ���� ������ Ŭ���̾�Ʈ���� ������
		for (auto i = clients.begin(); i != clients.end(); ++i)
			send_add_packet(p_id, i->first);

		//�̹������� Ŭ���̾�Ʈ���� �߰��� Ŭ���̾�Ʈ���� ������
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
		while (true); //����������� �����̾���.
	}
}
int main()
{
	wcout.imbue(std::locale("korean"));
	int retval = 0;

	// ���� �ʱ�ȭ
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
	// ������ ��ſ� ����� ����


	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);


	while (true)
	{
		SOCKET c_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (c_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}


		// ������ Ŭ���̾�Ʈ ���� ���
		/*printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/


		//Accept ó��
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
			//	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			//#endif // _DEBUG
			closesocket(c_sock);
		}


	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
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
//		cout << "��������" << endl;
//#endif // _DEBUG

		send_remove_packet(socketInfo->m_id);
		closesocket(clients[socketInfo->m_id].m_socket);
		clients.erase(socketInfo->m_id);
		return;
	}

	unsigned char* packet_ptr = socketInfo->m_buf;
	int key = socketInfo->m_id;
	int num_data = dataBytes + clients[key].m_prev_size; //ó���ؾ��� �������� ũ��=���ݹ���������+�������� ������������ ������

	int packet_size = packet_ptr[0]; //���� ó���ؾ��ϴ� ��Ŷ�� ũ��

	while (num_data >= packet_size) {
		proccess_packet(key, packet_ptr);
		num_data -= packet_size;
		packet_ptr += packet_size;
		if (0 >= num_data)break;//�������� ��ߵ�
		packet_size = packet_ptr[0];
	}
	clients[key].m_prev_size = num_data;//���� ��� ����
	if (0 != num_data)
		memcpy(socketInfo->m_buf, packet_ptr, sizeof(num_data));//�Ǿ����� ����

	do_recv(key);

}