#pragma once
class GameObj;
class GameMgr
{
public:
	static GameMgr& instance(HDC hdc, HWND hwnd, HINSTANCE hInst, char* serverIp);
	GameMgr() {};
	GameMgr(HDC hdc, HWND hwnd, HINSTANCE hInst,char* serverIp);
	void Draw();
	void keyDown(int key);

	void RecvData();
	
	void send_move_packet(DIRECTION dir);
	void send_packet(void* p);
	void send_login_packet();
	void processData(char* buf, size_t bufsize);

	void ProcessPacket(char* p);
	virtual ~GameMgr()
	{
		DeleteDC(m_memDC);
	}
private:
	HINSTANCE m_hInst;
	HDC m_hDC, m_memDC;
	PAINTSTRUCT m_ps;
	HBITMAP m_hMap, hChessObj,hOther;
	HWND m_hwnd;
	RECT m_winRect;
	int myId = -1;
	GameObj* m_pObjects=NULL;


	
	WSADATA m_wsa;
	SOCKET m_sock;
};

