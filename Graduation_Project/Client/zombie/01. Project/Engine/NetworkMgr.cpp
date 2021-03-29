#include "stdafx.h"
#include "NetworkMgr.h"
#include "Protocol.h"
#include "Component.h"
#include "Transform.h"
void CNetworkMgr::init()
{
	WSAStartup(MAKEWORD(2, 2), &m_wsa);


	int retval = 0;
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)err_display("socket()");

	//connec();
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(m_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_display("connect()");

	//최초 초기화 좌표 받기
	//this->RecvData();
}

void CNetworkMgr::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayerObject = pPlayer;
}

void CNetworkMgr::SendData(char key)
{
	c2s_Key packet;
	packet.size = sizeof(packet);
	packet.type = key;
	send(m_sock, reinterpret_cast<char*>(&packet),packet.size, 0);

}


Vec3 CNetworkMgr::RecvData()
{
	ZeroMemory(recv_buf, 0);
	Vec3 pos = { 0.f,0.f,0.f };
	int retval = recv(m_sock, recv_buf, sizeof(recv_buf) - 1, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
	}
	// recv_buf[retval] = '\0';
	switch (recv_buf[1])
	{
	case S2C_MOVE:
	{
		if (m_pPlayerObject != nullptr) {
			s2c_move* data = reinterpret_cast<s2c_move*>(recv_buf);
			Vec3 pos = { data->x,data->y,data->z };
		}
	}
	default:
		break;
	}
	return pos;
}

void CNetworkMgr::err_display(char* msg)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}
