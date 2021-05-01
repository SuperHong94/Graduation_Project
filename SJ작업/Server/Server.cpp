#include "stdafx.h"
#include "Data.h"


Data netData = { 0,0 };
int sizeData = sizeof(netData);
void Update(char* buf, int bufLength, int id);

int g_client_cnt;

map <SOCKET, SOCKETINFO> clients; //id�� �������������� Ŭ���̾�Ʈ��
map<SOCKET, Data> netDatas;
map<SOCKET, keyData> keyDatas;
void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{

	SOCKET  client_id = reinterpret_cast<int>(overlapped->hEvent);
	if (dataBytes == 0) {
		closesocket(clients[client_id].socket);
		clients.erase(client_id);
		return;
	}
	//[client_id].messageBuffer[dataBytes]
	clients[client_id].messageBuffer[dataBytes] = 0; //�޸� ������ ������ �پ ������ 0�����Ѵ�.

	memset(&(clients[client_id].overlapped), 0, sizeof(WSAOVERLAPPED)); //0�����ʱ�ȭ�ϴµ� �̺�Ʈ�� 0���� �ʱ�ȭ��
	Update(clients[client_id].messageBuffer, dataBytes, client_id);

	clients[client_id].overlapped.hEvent = (HANDLE)client_id; //�ٽ��̺�Ʈ�� ��
	WSASend(client_id, &(clients[client_id].dataBuffer), 1, NULL, 0, &(clients[client_id].overlapped), send_callback);


}

void send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD receiveBytes = 0;
	DWORD flags = 0;

	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���

	clients[client_s].dataBuffer.len = BUFSIZE;
	memset(&(clients[client_s].overlapped), 0, sizeof(WSAOVERLAPPED));
	clients[client_s].overlapped.hEvent = (HANDLE)client_s;
	WSARecv(client_s, &clients[client_s].dataBuffer, 1, 0, &flags, &(clients[client_s].overlapped), recv_callback);

}

int main()
{
	int retval = 0;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET)
		err_quit("WSAsocket()");

	// bind() n

	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(SOCKADDR_IN));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	char buf[BUFSIZE + 1];

	ZeroMemory(buf, sizeof(buf));

	g_client_cnt = -1;
	while (1)
	{
		SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			return 0;
		}

		clients[client_sock] = SOCKETINFO{};
		clients[client_sock].socket = client_sock;
		clients[client_sock].dataBuffer.len = BUFSIZE;
		clients[client_sock].dataBuffer.buf = clients[client_sock].messageBuffer;

		memset(&clients[client_sock].overlapped, 0, sizeof(WSAOVERLAPPED));
		clients[client_sock].overlapped.hEvent = (HANDLE)clients[client_sock].socket;

		DWORD flags = 0;

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//Ű������ �ޱ�
		retval = WSARecv(clients[client_sock].socket, &clients[client_sock].dataBuffer, 1, NULL,
			&flags, &(clients[client_sock].overlapped), recv_callback);

	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}


void Update(char* buf, int bufLength, int id)
{
	int offX = 28;
	int offY = 28;


	for (int i = 0; i < bufLength; i++)
	{
		switch (buf[i])
		{

			cout << buf[i] << '\n';

		case 'w':
		case 'W':
			if (netDatas[id].y - offY > -20)
				netDatas[id].y = netDatas[id].y - offY;
			break;
		case 'a':
		case 'A':
			if (netDatas[id].x - offX > -20)
				netDatas[id].x = netDatas[id].x - offX;
			break;
		case 's':
		case 'S':
			if (netDatas[id].y + offY < 200) //225�� ���� ũ����
				netDatas[id].y = netDatas[id].y + offY;
			break;
		case 'd':
		case 'D':
			if (netDatas[id].x + offX < 200)
				netDatas[id].x = netDatas[id].x + offX;
			break;
		default:
			break;
		}
	}
}