#pragma once
#include "GameObject.h"
class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:

	
	WSADATA m_wsa;
	SOCKET m_sock;
	char recv_buf[BUFSIZ];


	CGameObject* m_pPlayerObject;
public:

	//static CNetworkMgr* GetInst()
	//{

	//}
	void init();
	void SetPlayer(CGameObject* pPlayer);
	void SendData(char key);
	Vec3 RecvData();
	void err_display(char* msg);
};

//class CKeyMgr
//{
//	SINGLE(CKeyMgr);
//private:
//	vector<tKeyState>	m_vecKey;
//	POINT				m_ptMouse;
//	POINT				m_ptOldMouse;
//
//	Vec2				m_vDragDir;
//
//public:
//	void init();
//	void update();
//
//public:
//	KEY_STATE GetKeyState(KEY_TYPE _eKey) { return m_vecKey[(UINT)_eKey].eState; }
//	const POINT& GetMousePos() { return m_ptMouse; }
//	const POINT& GetPrevMousePos() { return m_ptOldMouse; }
//	Vec2 GetDragDir() { return m_vDragDir; }
//};
//
