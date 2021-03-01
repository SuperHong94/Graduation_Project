#include "pch.h"
#include "GameMgr.h"
#include "GameObj.h"

GameMgr& GameMgr::instance(HDC hdc, HWND hwnd, HINSTANCE hInst)
{
	static GameMgr* instance = new GameMgr(hdc, hwnd, hInst);
	return *instance;

}

void GameMgr::Draw()
{
	m_hDC = BeginPaint(m_hwnd, &m_ps);


	m_memDC = CreateCompatibleDC(m_hDC);
	SelectObject(m_memDC, m_hMap);
	BitBlt(m_hDC, 0, 0, 225, 225, m_memDC, 0, 0, SRCCOPY);
	SelectObject(m_memDC, hChessObj);
	for (int i = 0; i < m_nObjects; i++)
		m_pObjects[i].Draw(m_hDC, m_memDC);

	DeleteDC(m_memDC);
	EndPaint(m_hwnd, &m_ps);
}

void GameMgr::keyDown(int key)
{

	m_pKeys[m_curKeyCNT] = key;
	m_curKeyCNT++;


	InvalidateRect(m_hwnd, &m_winRect, TRUE); //강제로 paint메시지호출
}

void GameMgr::SendData()
{
	if (m_pKeys[0] == 0) {
		m_pKeys[1] = 'f';
	}
	if (send(m_sock, m_pKeys, m_curKeyCNT, 0) == SOCKET_ERROR)
	{
		err_display("send()");
	}

}

void GameMgr::RecvData()
{
	char buffer[500];
	int retval= recv(m_sock, buffer, sizeof(buffer) - 1, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv");
	}
	buffer[retval] = '\0';

	Data* data = (Data*)buffer;

	for (int i = 0; i < m_nObjects; i++)
		m_pObjects[i].SetPos(data->x,data->y);

}

GameMgr::GameMgr(HDC hdc, HWND hwnd, HINSTANCE hInst) :m_hDC(hdc), m_hwnd(hwnd), m_hInst(hInst)
{

	m_hMap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAPBOARD));
	hChessObj = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CHESSOBJECT));
	m_winRect = { 0,0,WINWIDTH,WINHEIGHT };
	m_pObjects = new GameObj;

	m_curKeyCNT = 0;
	ZeroMemory(m_pKeys, sizeof(m_pKeys));



	m_wsa;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0){
		err_quit("WSAStartup");
	}
	// socket()

	int retval = 0;
	m_sock = WSASocket(AF_INET, SOCK_STREAM, 0,NULL,0,0);
	if (m_sock == INVALID_SOCKET)
	{
		err_quit("socket()");
	}


	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	retval = connect(m_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) err_quit("connect()");


}
