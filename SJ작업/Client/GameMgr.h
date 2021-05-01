#pragma once
class GameObj;
class GameMgr
{
public:
	static GameMgr& instance(HDC hdc, HWND hwnd, HINSTANCE hInst);
	GameMgr() {};
	GameMgr(HDC hdc, HWND hwnd, HINSTANCE hInst);
	void Draw();
	void keyDown(int key);
	void SendData();
	void RecvData();
	virtual ~GameMgr()
	{
		DeleteDC(m_memDC);
	}
private:
	HINSTANCE m_hInst;
	HDC m_hDC, m_memDC;
	PAINTSTRUCT m_ps;
	HBITMAP m_hMap, hChessObj;
	HWND m_hwnd;
	RECT m_winRect;
	int m_nObjects = 1;
	GameObj* m_pObjects=NULL;

	USHORT m_curKeyCNT = 0;
	char m_pKeys[BUFSIZE];
	
	
	
	WSADATA m_wsa;
	SOCKET m_sock;
};

