#pragma once

class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:
	WSADATA m_wsa;
	SOCKET m_sock;
	CGameObject* m_pPlayer;

	std::array<CGameObject*, MAX_USER> m_PlayerArray;
public:
	void init();
	void err_display(const char* msg, int error);

	/// <summary>
	/// ���ú��ϱ�
	/// </summary>
	void client_main();

	void processPacket(char* buf, DWORD bufsize);

	void process(char* buf);
	void process_key(s2c_move* p);
	void send_login_packet();
	void send_packet(void* packet);

	void send_Key_packet(EKEY_EVENT key, Vec3 Rotation);

public: //������Ʈ ���� �޼ҵ�

	std::array<CGameObject*, MAX_USER> GetPlayerArray() { return m_PlayerArray; };
	void SetPlayer(CGameObject* pPlayer);

	bool m_isChange = false;

	Vec3 playerPos;
	PlayerState m_ePState;

};


