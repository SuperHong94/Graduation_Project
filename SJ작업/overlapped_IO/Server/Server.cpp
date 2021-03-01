#include "stdafx.h"
#include "Data.h"

Data netData = { 0,0 };
int sizeData = sizeof(netData);
void Update(char* buf, int bufLength,unsigned int id);



map<unsigned int, SOCKETINFO> clients; //id와 소켓인포를가진 클라이언트들
map<unsigned int, Data> netDatas;
map<unsigned int, keyData> keyDatas;
void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	unsigned int client_id = reinterpret_cast<unsigned int>(overlapped->hEvent);
	if (dataBytes == 0) {
		closesocket(clients[client_id].socket);
		clients.erase(client_id);
		return;
	}
	//[client_id].messageBuffer[dataBytes]
	Update(clients[client_id].messageBuffer, dataBytes, client_id);
	for (auto i = clients.begin(); i != clients.end(); ++i)
	{
		
	}
}

int main()
{
	int retval = 0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, 0,NULL,0,WSA_FLAG_OVERLAPPED);
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

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	char buf[BUFSIZE + 1];

	ZeroMemory(buf, sizeof(buf));

	int client_cnt = -1;
	while (1)
	{
		SOCKET client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		else
			client_cnt += 1;
		clients[client_cnt] = SOCKETINFO{};
		clients[client_cnt].socket = client_sock;
		clients[client_cnt].dataBuffer.len = BUFSIZE;
		clients[client_cnt].dataBuffer.buf = clients[client_cnt].messageBuffer;
		memset(&clients[client_cnt].overlapped, 0, sizeof(WSAOVERLAPPED));
		clients[client_cnt].overlapped.hEvent = (HANDLE)clients[client_cnt].socket;

		DWORD flags = 0;

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//키데이터 받기
		retval = WSARecv(clients[client_cnt].socket, &clients[client_cnt].dataBuffer, 1, NULL,
			&flags, &(clients[client_cnt].overlapped), recv_callback);
		// 클라이언트와 데이터 통신
		//while (1) {
		//	//  키데이터 받기
		//	retval = recv(client_sock, buf, BUFSIZE, 0);
		//	if (retval == SOCKET_ERROR) {
		//		err_display("recv()");
		//		break;
		//	}
		//	//Update(buf, BUFSIZE);

		//	/*retval = send(client_sock, (char*)&netData, sizeData, 0);
		//	if (retval == SOCKET_ERROR)
		//	{
		//		err_display("send()");
		//	}
		//	if (retval > 0)
		//	{
		//		cout<<"x:" <<netData.x<<" y:"<<netData.y<< " 데이터를 보냈습니다. " << endl;
		//	}*/

		//}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}


void Update(char* buf, int bufLength, unsigned int id)
{
	int offX = 28;
	int offY = 28;
	
	for (int i = 0; i < bufLength; i++)
	{
		switch (buf[i])
		{
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
			if (netDatas[id].y + offY < 200) //225는 보드 크기임
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