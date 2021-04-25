#pragma once

class CGameObject;
class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:
	WSADATA m_wsa;
	SOCKET m_sock;
	CGameObject* m_pPlayer;

	CGameObject** m_pPlayerArray;

	int m_id;
public:
	void init();
	void err_display(const char* msg, int error);

	/// <summary>
	/// 리시브하기
	/// </summary>
	void client_main();

	void processPacket(char* buf, DWORD bufsize);

	void process(char* buf);
	void process_key(s2c_move* p);
	void send_login_packet();
	void send_packet(void* packet);

	void send_Key_packet(EKEY_EVENT key, Vec3 Rotation);

public: //오브젝트 관련 메소드

	CGameObject** GetPlayerArray() { return m_pPlayerArray; }; //참조자 리턴 밖에서 수정가능
	void SetPlayerArray(CGameObject** playerArray) { m_pPlayerArray = playerArray; };
	void SetPlayer(CGameObject* pPlayer);

	int GetId() { return m_id; };
	bool m_isChange = false;

	Vec3 playerPos;
	PlayerState m_ePState;

};


