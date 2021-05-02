#include "pch.h"
#include "GameMgr.h"
#include "GameObj.h"

GameMgr& GameMgr::instance(HDC hdc, HWND hwnd, HINSTANCE hInst, char* serverIP)
{
	static GameMgr* instance = new GameMgr(hdc, hwnd, hInst, serverIP);
	return *instance;

}

void GameMgr::Draw()
{
	m_hDC = BeginPaint(m_hwnd, &m_ps);


	m_memDC = CreateCompatibleDC(m_hDC);
	SelectObject(m_memDC, m_hMap);
	BitBlt(m_hDC, 0, 0, 225, 225, m_memDC, 0, 0, SRCCOPY);
	SelectObject(m_memDC, hChessObj);
	for (int i = 0; i < MAX_USER; i++)
		if (m_pObjects[i].isVisible)
			if (m_pObjects[i].isMe)
				m_pObjects[i].Draw(m_hDC, m_memDC);

	SelectObject(m_memDC, hOther);
	for (int i = 0; i < MAX_USER; i++)
		if (m_pObjects[i].isVisible)
			if (!m_pObjects[i].isMe)
				m_pObjects[i].Draw(m_hDC, m_memDC);

	DeleteDC(m_memDC);
	EndPaint(m_hwnd, &m_ps);
}

void GameMgr::keyDown(int key)
{

	switch (key)
	{
	case VK_UP:
		send_move_packet(D_N);
		break;
	case VK_DOWN:
		send_move_packet(D_S);
		break;
	case VK_LEFT:
		send_move_packet(D_W);
		break;
	case VK_RIGHT:
		send_move_packet(D_E);
		break;
	default:
		break;
	}


	//InvalidateRect(m_hwnd, &m_winRect, TRUE); //강제로 paint메시지호출
}


void GameMgr::RecvData()
{
	static char buffer[MAX_BUFFER];
	int retval = recv(m_sock, buffer, sizeof(buffer), 0);
	if (retval == SOCKET_ERROR)
	{
		return;
	}
	if (retval > 0)
		processData(buffer, retval);

	//InvalidateRect(m_hwnd, &m_winRect, TRUE); //강제로 paint메시지호출
}

void GameMgr::send_move_packet(DIRECTION dir)
{
	c2s_move packet;
	packet.size = sizeof(packet);
	packet.type = C2S_MOVE;
	packet.dr = dir;
	size_t sent = 0;

	send_packet(&packet);

}

void GameMgr::send_packet(void* p)
{
	int p_size = reinterpret_cast<unsigned char*>(p)[0];
	//int p_type = reinterpret_cast<unsigned char*>(p)[1];

	char* buf = reinterpret_cast<char*>(p);
	int retval = send(m_sock, buf, p_size, 0);


}

void GameMgr::send_login_packet()
{
	c2s_login packet;
	packet.size = sizeof(packet);
	packet.type = C2S_LOGIN;
	send_packet(&packet);
}

void GameMgr::processData(char* buf, size_t bufsize)
{
	char* ptr = buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[MAX_BUFFER];

	while (0 != bufsize) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (bufsize + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			bufsize -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, bufsize);
			saved_packet_size += bufsize;
			bufsize = 0;
		}
	}
}

void GameMgr::ProcessPacket(char* ptr)
{
	switch (ptr[1])
	{
	case S2C_LOGIN_OK:
	{
		s2c_login_ok* packet = reinterpret_cast<s2c_login_ok*>(ptr);
		myId = packet->id;
		m_pObjects[myId].SetPos(packet->x, packet->y);
		m_pObjects[myId].SetMe();
		m_pObjects[myId].isVisible = true;
	}
	break;
	case S2C_ADD_PLAYER:
	{
		s2c_add_player* packet = reinterpret_cast<s2c_add_player*>(ptr);
		int id = packet->id;

		if (id != myId) {
			m_pObjects[id].SetPos(packet->x, packet->y);
			m_pObjects[id].isVisible = true;
		}
		else {
			//npc[id - NPC_START].x = my_packet->x;
			//npc[id - NPC_START].y = my_packet->y;
			//npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
		}

	}
	break;
	case S2C_MOVE_PLAYER:
	{
		s2c_move_player* packet = reinterpret_cast<s2c_move_player*>(ptr);
		int id = packet->id;
		if (id == myId) {
			m_pObjects[id].SetPos(packet->x, packet->y);

		}
		else if (id != myId) {
			m_pObjects[id].SetPos(packet->x, packet->y);
		}
		else {
			//npc[other_id - NPC_START].x = my_packet->x;
			//npc[other_id - NPC_START].y = my_packet->y;
		}
		break;
	}

	case S2C_REMOVE_PLAYER:
	{
		s2c_remove_player* my_packet = reinterpret_cast<s2c_remove_player*>(ptr);
		int id = my_packet->id;
		if (id == myId) {
			m_pObjects[id].isVisible = false;
		}
		else if (id < MAX_USER) {
			m_pObjects[id].isVisible = false;
		}
		else {
			//		npc[other_id - NPC_START].attr &= ~BOB_ATTR_VISIBLE;
		}
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
	InvalidateRect(m_hwnd, &m_winRect, TRUE);
}



GameMgr::GameMgr(HDC hdc, HWND hwnd, HINSTANCE hInst, char* serverIp) :m_hDC(hdc), m_hwnd(hwnd), m_hInst(hInst)
{

	m_hMap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAPBOARD));
	hChessObj = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CHESSOBJECT));
	hOther = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OTHER));
	m_winRect = { 0,0,WINWIDTH,WINHEIGHT };

	m_pObjects = new GameObj[MAX_USER];
	memset(m_pObjects, 0, sizeof(m_pObjects));





	m_wsa;
	WSAStartup(MAKEWORD(2, 0), &m_wsa);
	// socket()

	int retval = 0;
	unsigned long noblock = 1;
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(m_sock, FIONBIO, &noblock);

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverIp);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(m_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	//로그인 보내기

}
