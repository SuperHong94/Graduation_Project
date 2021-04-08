#include "stdafx.h"
#include "NetworkMgr.h"
#include "GameObject.h"
#include "Transform.h" //이동을위한 헤더


//생성자 
CNetworkMgr::CNetworkMgr()
{};
CNetworkMgr::~CNetworkMgr()
{};

void CNetworkMgr::err_display(const char* msg, int error)
{

	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << lpMsgBuf << std::endl;
	//while (true);
	LocalFree(lpMsgBuf);
	while (true);//디버그해야함
}

void CNetworkMgr::client_main()
{
	char recvBuf[MAX_BUFFER];
	WSABUF recvWsaBuf[1];
	recvWsaBuf[0].buf = recvBuf;
	recvWsaBuf[0].len = MAX_BUFFER;

	DWORD dataBytes = 0;
	DWORD rFlag = 0;
	int retval = WSARecv(m_sock, recvWsaBuf, 1, &dataBytes, &rFlag, NULL, 0);
	if (retval != NO_ERROR)
	{
		int err_code = WSAGetLastError();
		if (err_code != WSAEWOULDBLOCK)
			err_display("WSARecv()", err_code);
	}
	if (dataBytes > 0)
		processPacket(recvBuf, dataBytes);
}

void CNetworkMgr::processPacket(char* buf, DWORD bufsize)
{
	char* ptr = buf;
	static DWORD inPacketSize = 0; //새패킷
	static DWORD savedPacketSize = 0; //잔여패킷
	static char packetBuffer[MAX_BUFFER];
	while (bufsize != 0)
	{
		if (inPacketSize == 0)
			inPacketSize = buf[0];
		if (bufsize + savedPacketSize >= inPacketSize)
		{
			memcpy(packetBuffer + savedPacketSize, ptr, inPacketSize - savedPacketSize);
			process(packetBuffer);
			ptr += inPacketSize - savedPacketSize;
			bufsize -= inPacketSize - savedPacketSize;
			inPacketSize = 0;
			savedPacketSize = 0;
		}
		else
		{
			memcpy(packetBuffer + savedPacketSize, ptr, bufsize);
			savedPacketSize += bufsize;
			bufsize = 0;
		}
	}
}

void CNetworkMgr::process(char* buf)
{


	switch (buf[1])//타입확인
	{
	case S2C_LOGIN_OK:
	{
		s2c_loginOK* p = reinterpret_cast<s2c_loginOK*>(buf);
		/*if (m_pPlayer != nullptr)
		{

			m_pPlayer->Transform()->SetLocalPos(Vec3(p->x, p->y, p->z));
		}*/
		playerPos = Vec3(p->x, p->y, p->z);

#ifdef _DEBUG
		std::cout << "서버에서 접속 성공 초기좌표" << p->x << ',' << p->y << ',' << p->z << std::endl;
#endif // _DEBUG

		//std::cout << p->x << ' ' << p->y << ' ' << p->z << std::endl;
	}
	break;
	case S2C_MOVE:
	{
		s2c_move* p = reinterpret_cast<s2c_move*>(buf);

#ifdef _DEBUG
		std::cout << "키 정보결과로 " << p->x << ',' << p->y << ',' << p->z << std::endl;
#endif // _DEBUG

		//if (m_pPlayer != nullptr)
		//{
		//	Vec3 vPos = Vec3(p->x, p->y, p->z);
		//	//m_pPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM)->Transform()->SetLocalPos(vPos);
		//	m_pPlayer->Transform()->SetLocalPos(vPos);
		//}
		playerPos = Vec3(p->x, p->y, p->z);
	}
	break;
	default:
		break;
	}
}

void CNetworkMgr::send_login_packet()
{
	c2s_login packet;
	packet.size = sizeof(packet);
	packet.type = C2S_LOGIN;
	send_packet(&packet);
}

void CNetworkMgr::send_packet(void* packet)
{
	int SendSize = reinterpret_cast<unsigned char*>(packet)[0];
	char* buf = reinterpret_cast<char*>(packet);
	WSABUF sendWsaBuf[1];
	sendWsaBuf[0].buf = buf;
	sendWsaBuf[0].len = SendSize;
	DWORD databytes = 0;
	DWORD sFlag = 0;
	int retval = WSASend(m_sock, sendWsaBuf, 1, &databytes, sFlag, NULL, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("WSASend()", WSAGetLastError());
	}
}

void CNetworkMgr::send_Key_packet(EKEY_EVENT key)
{
	c2s_Key packet;
	packet.type = C2S_KEY_EVENT;
	packet.size = sizeof(packet);
	packet.key = key;

#ifdef _DEBUG
	std::cout << "서버에게" << key << " 키 정보를 보낸다.\n";
#endif // _DEBUG

	send_packet(&packet);
}

void CNetworkMgr::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
}




void CNetworkMgr::init()
{
	m_pPlayer = nullptr;
	using namespace std;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		cout << "WSAStartUp 실패" << endl;
		exit(-1); //종료시키기
	}

	m_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, NULL);
	if (m_sock == INVALID_SOCKET)
	{
		err_display("WSASocket()", WSAGetLastError());
		exit(-1);
	}

	//논블록 소켓으로 설정
	unsigned long noblock = 1;
	int retval = ioctlsocket(m_sock, FIONBIO, &noblock);
	if (retval != NO_ERROR)
	{
		err_display("ioctlsocket", WSAGetLastError());
		exit(-1);
	}
	//connect()
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr);
	retval = connect(m_sock, reinterpret_cast<SOCKADDR*>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		int errono = WSAGetLastError();

		if (errono != WSAEWOULDBLOCK) {
			err_display("connect()", errono);
			exit(-1);
		}
	}
	else
#ifdef _DEBUG
		cout << "서버와 connect 성공" << endl;
#endif // _DEBUG

	send_login_packet();
	client_main();
}
