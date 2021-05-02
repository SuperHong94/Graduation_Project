#pragma once

class CGameObject;
class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:
	WSADATA m_wsa;
	SOCKET m_sock;

	CGameObject* m_pPlayerArray[MAX_USER];

	int m_id;
	int m_playerId;
	SCENE_STATE m_eSceneState;
public:
	void init();
	void init_game();// 게임씬  들어가면 하는 초기화
	void err_display(const char* msg, int error);

	/// <summary>
	/// 리시브하기
	/// </summary>
	void client_main();

	void processPacket(char* buf, DWORD bufsize);

	void UpdateScene();
	void process(char* buf);
	void process_key(s2c_move* p);
	void send_login_packet();
	void send_packet(void* packet);
	void send_chage_scene();
	void send_Key_packet(EKEY_EVENT key, Vec3 Rotation);

public: //오브젝트 관련 메소드

	CGameObject** GetPlayerArray() { return m_pPlayerArray; }; //참조자 리턴 밖에서 수정가능
	//void SetPlayerArray(CGameObject* playerArray[MAX_USER]) { m_pPlayerArray = playerArray; };
	//void SetPlayerArray(CGameObject playerArray[][MAX_USER]) { m_pPlayerArray = playerArray; };
	int GetId() { return m_id; };
	int GetPlayerId() { return m_playerId; };
	SCENE_STATE GetSceneState() { return m_eSceneState; };
	bool m_isChange = false;

	Vec3 playerPos;
	PlayerState m_ePState;


};


