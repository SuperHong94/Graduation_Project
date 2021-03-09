#include "stdafx.h"
#include "Data.h"


Data netData = { 0,0 };
int sizeData = sizeof(netData);
void Update(char* buf, int bufLength, unsigned int id);

int g_client_cnt;

map <SOCKET, SOCKETINFO> clients; //id와 소켓인포를가진 클라이언트들
map<unsigned int, Data> netDatas;
map<unsigned int, keyData> keyDatas;
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
	clients[client_id].messageBuffer[dataBytes] = 0; //메모리 마지막 쓰레기 붙어서 마지막 0으로한다.

	memset(&(clients[client_id].overlapped), 0, sizeof(WSAOVERLAPPED)); //0으로초기화하는데 이벤트도 0으로 초기화됨
	Update(clients[client_id].messageBuffer, dataBytes, client_id);

	clients[client_id].overlapped.hEvent = (HANDLE)client_id; //다시이벤트도 설
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
	}  // 클라이언트가 closesocket을 했을 경우

	clients[client_s].dataBuffer.len = BUFSIZE;
	memset(&(clients[client_s].overlapped), 0, sizeof(WSAOVERLAPPED));
	clients[client_s].overlapped.hEvent = (HANDLE)client_s;
	WSARecv(client_s, &clients[client_s].dataBuffer, 1, 0, &flags, &(clients[client_s].overlapped), recv_callback);

}

int main()
{
	int retval = 0;

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
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
			break;
		}
	
		clients[client_sock] = SOCKETINFO{};
		clients[client_sock].socket = client_sock;
		clients[client_sock].dataBuffer.len = BUFSIZE;
		clients[client_sock].dataBuffer.buf = clients[g_client_cnt].messageBuffer;
		memset(&clients[client_sock].overlapped, 0, sizeof(WSAOVERLAPPED));
		clients[client_sock].overlapped.hEvent = (HANDLE)clients[client_sock].socket;

		DWORD flags = 0;

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//키데이터 받기
		retval = WSARecv(clients[client_sock].socket, &clients[client_sock].dataBuffer, 1, NULL,
			&flags, &(clients[client_sock].overlapped), recv_callback);


		//업데이트하고 모든 클라이언트에게 정보 보내기
		//retval=WSASend(clients[client_sock].socket,&clients[client_sock].dataBuffer,1,NULL,&flags, &(clients[client_sock].overlapped),send_callback)

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
		/*closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/
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