#pragma once
class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:
	WSADATA m_wsa;
	SOCKET m_sock;
	CGameObject* m_pPlayer;
public:
	void init();
	void err_display(const char* msg, int error);

	/// <summary>
	/// ���ú��ϱ�
	/// </summary>
	void client_main();

	void processPacket(char* buf, DWORD bufsize);

	void process(char* buf);
	void send_login_packet();
	void send_packet(void* packet);
	void send_Key_packet(EKEY_EVENT key);


public: //������Ʈ ���� �޼ҵ�
	void SetPlayer(CGameObject* pPlayer);

	Vec3 playerPos;

};


