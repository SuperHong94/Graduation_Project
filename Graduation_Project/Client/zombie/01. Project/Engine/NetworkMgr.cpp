#include "stdafx.h"
#include "NetworkMgr.h"
#include "GameObject.h"
#include "Transform.h" //이동을위한 헤더
#include "TimeMgr.h" //DT를 위해서
#include "PlayerScript.h"	
<<<<<<< HEAD
#include "SceneMgr.h"
=======
>>>>>>> yjs
//생성자 
CNetworkMgr::CNetworkMgr()
{};
CNetworkMgr::~CNetworkMgr()
{};

void CNetworkMgr::err_display(const char* msg, int error)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << lpMsgBuf << std::endl;
	//while (true);
	LocalFree(lpMsgBuf);
	while (true);//디버그해야함
}

void CNetworkMgr::client_main()
{

	char recvBuf[MAX_BUFFER];
	WSABUF recvWsaBuf[1];
	recvWsaBuf[0].buf = recvBuf;
	recvWsaBuf[0].len = MAX_BUFFER;

	DWORD dataBytes = 0;
	DWORD rFlag = 0;
	int retval = WSARecv(m_sock, recvWsaBuf, 1, &dataBytes, &rFlag, NULL, 0);
	if (retval != NO_ERROR)
	{
		int err_code = WSAGetLastError();
		if (err_code != WSAEWOULDBLOCK)
			err_display("WSARecv()", err_code);
	}
	if (dataBytes > 0)
		processPacket(recvBuf, dataBytes);
}

void CNetworkMgr::processPacket(char* buf, DWORD bufsize)
{
	char* ptr = buf;
	static DWORD inPacketSize = 0; //새패킷
	static DWORD savedPacketSize = 0; //잔여패킷
	static char packetBuffer[MAX_BUFFER];
	while (bufsize != 0)
	{
		if (inPacketSize == 0)
			inPacketSize = buf[0];
		if (bufsize + savedPacketSize >= inPacketSize)
		{
			memcpy(packetBuffer + savedPacketSize, ptr, inPacketSize - savedPacketSize);
			process(packetBuffer);
			ptr += inPacketSize - savedPacketSize;
			bufsize -= inPacketSize - savedPacketSize;
			inPacketSize = 0;
			savedPacketSize = 0;
		}
		else
		{
			memcpy(packetBuffer + savedPacketSize, ptr, bufsize);
			savedPacketSize += bufsize;
			bufsize = 0;
		}
	}
}

<<<<<<< HEAD
void CNetworkMgr::UpdateScene()
{
	switch (m_eSceneState)
	{
	case SCENE_STATE::START_SCENE:
		break;
	case SCENE_STATE::GAME_SCENE:
		init_game();
		CSceneMgr::GetInst()->NewCurScene();
		CSceneMgr::GetInst()->init();
		CSceneMgr::GetInst()->SetIsChange(true);
		break;
	case SCENE_STATE::GAMECLEAR_SCENE:
		break;
	case SCENE_STATE::GAMEOVER_SCENE:
		break;
	default:
		break;
	}
}

=======
>>>>>>> yjs
void CNetworkMgr::process(char* buf)
{


	switch (buf[1])//타입확인
	{
	case S2C_LOGIN_OK:
	{
		s2c_loginOK* p = reinterpret_cast<s2c_loginOK*>(buf);

<<<<<<< HEAD

		m_id = p->id; //이것은 통신하기위한 ID
		m_playerId = m_id - 1; //이것은 그리기위한 ID 이렇게 한이유는 m_id의 0은 서버ID이다. 하지만 랜더링을 위한 플레이어 배열의 시작은 0부터시작하기에 이렇게 하였다.
=======
		
		m_id = p->id - 1; //0번은 서버아이디여서 할당안되지만 게임오브젝트 배열은 0번부터시작

>>>>>>> yjs
		//로그인 확인하고 논블로킹 소켓으로 바꿈
		unsigned long noblock = 1;
		int retval = ioctlsocket(m_sock, FIONBIO, &noblock);
		if (retval != NO_ERROR)
		{
			err_display("ioctlsocket", WSAGetLastError());
			exit(-1);
		}
<<<<<<< HEAD

	}
	break;
	case S2C_CHANGE_SCENE:
	{
		std::cout << "씬변환 받음\n";
		s2c_change_Scene* packet = reinterpret_cast<s2c_change_Scene*>(buf);
		m_eSceneState = packet->eScene_state;
		UpdateScene();
	}
	break;
=======
#ifdef _DEBUG
		std::cout << "서버에서 접속 성공 초기좌표" << p->x << ',' << p->y << ',' << p->z << std::endl;
#endif // _DEBUG

	}
	break;
>>>>>>> yjs
	case S2C_ADD_PLAYER: //추가접속플레이어
	{
		s2c_add_client* packet = reinterpret_cast<s2c_add_client*>(buf);
		int id = packet->id - 1;
		if (m_pPlayerArray != nullptr) {
<<<<<<< HEAD

			m_pPlayerArray[id]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = false;
			m_pPlayerArray[id]->GetScript<CPlayerScript>()->Transform()->SetLocalPos(Vec3(packet->x,packet->y,packet->z));
=======
			m_pPlayerArray[id]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = true;
			m_pPlayerArray[id]->GetScript<CPlayerScript>()->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
>>>>>>> yjs
		}
	}
	break;
	case S2C_MOVE:
	{

		s2c_move* p = reinterpret_cast<s2c_move*>(buf);

		process_key(p);
#ifdef _DEBUG
		std::cout << "키 정보결과로 " << p->x << ',' << p->y << ',' << p->z << "를 받았다" << std::endl;
#endif 
	}
	break;
	default:
		break;
	}
}

void CNetworkMgr::process_key(s2c_move* p)
{

	Vec3 packetPos = { p->x,p->y,p->z };
	if (packetPos == playerPos)
		m_isChange = false;
	else
		m_isChange = true;

	if (m_isChange) {
		playerPos = packetPos;
		m_ePState = p->ePlayerState;
	}

}

void CNetworkMgr::send_login_packet()
{
	c2s_login packet;
	packet.size = sizeof(packet);
	packet.type = C2S_LOGIN;
	send_packet(&packet);
}

void CNetworkMgr::send_packet(void* packet)
{
	int SendSize = reinterpret_cast<unsigned char*>(packet)[0];
	char* buf = reinterpret_cast<char*>(packet);
	WSABUF sendWsaBuf[1];
	sendWsaBuf[0].buf = buf;
	sendWsaBuf[0].len = SendSize;
	DWORD databytes = 0;
	DWORD sFlag = 0;
	int retval = WSASend(m_sock, sendWsaBuf, 1, &databytes, sFlag, NULL, 0);
	if (retval != NO_ERROR)
	{
		err_display("WSASend()", WSAGetLastError());
	}
}

<<<<<<< HEAD
void CNetworkMgr::send_chage_scene()
{
	c2s_chage_scene packet;
	packet.size = sizeof(packet);
	packet.type = C2S_CHANGE_SCENE;
	packet.id = m_id;
	packet.eSceneStatae = m_eSceneState;
	send_packet(&packet);
}

=======
>>>>>>> yjs


void CNetworkMgr::send_Key_packet(EKEY_EVENT key, Vec3 Rotation)
{
	c2s_Key packet;
	packet.type = C2S_KEY_EVENT;
	packet.size = sizeof(packet);
	packet.key = key;
	packet.dT = DT;
	packet.rX = Rotation.x;
	packet.rY = Rotation.y;
	packet.rZ = Rotation.z;
	//#ifdef _DEBUG
	//	std::cout << "서버에게" << key << " 키 정보를 보낸다.\n";
	//#endif // _DEBUG

	send_packet(&packet);
}

<<<<<<< HEAD
=======
void CNetworkMgr::SetPlayer(CGameObject* pPlayer)
{
	m_pPlayer = pPlayer;
}
>>>>>>> yjs




void CNetworkMgr::init()
{
<<<<<<< HEAD

	//m_pPlayerArray = nullptr;
	//for (int i = 0; i < MAX_USER; ++i) {
	//	m_pPlayerArray[i] = nullptr;
	//}
=======
	
	m_pPlayerArray = nullptr;
	for (int i = 0; i < MAX_USER; ++i) {
		m_pPlayerArray[i] = nullptr;
	}
>>>>>>> yjs
	using namespace std;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		cout << "WSAStartUp 실패" << endl;
		exit(-1); //종료시키기
	}

	m_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, NULL);
	if (m_sock == INVALID_SOCKET)
	{
		err_display("WSASocket()", WSAGetLastError());
		exit(-1);
	}

	//논블록 소켓으로 설정
	/*unsigned long noblock = 1;
	int retval = ioctlsocket(m_sock, FIONBIO, &noblock);
	if (retval != NO_ERROR)
	{
		err_display("ioctlsocket", WSAGetLastError());
		exit(-1);
	}*/
	//connect()
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr);
	int retval = connect(m_sock, reinterpret_cast<SOCKADDR*>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		int errono = WSAGetLastError();

		if (errono != WSAEWOULDBLOCK) {
			err_display("connect()", errono);
			exit(-1);
		}
	}

#ifdef _DEBUG
	cout << "서버와 connect 성공" << endl;
#endif // _DEBUG

	send_login_packet();
	//client_main();
}
<<<<<<< HEAD

void CNetworkMgr::init_game()
{
	Ptr<CMeshData> pMeshData;
	//m_pPlayerArray = new CGameObject * [MAX_USER];
	for (int i = 0; i < MAX_USER; i++)
	{
		m_pPlayerArray[i] = new CGameObject;

		// 모델을 플레이어별로 따로 설정할수도 있음
		// 아직 보류
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
		m_pPlayerArray[i] = pMeshData->Instantiate();

		m_pPlayerArray[i]->SetName(L"Player Object");
		m_pPlayerArray[i]->AddComponent(new CTransform);
		//pPlayerObject->AddComponent(new CMeshRender);

		// Transform 설정

		if (i == m_playerId)
			m_pPlayerArray[i]->Transform()->SetLocalPos(Vec3(0, 0.f, 0.f));
		else
			m_pPlayerArray[i]->Transform()->SetLocalPos(Vec3(-20000.f, 20000.f, -20000.f));


		m_pPlayerArray[i]->Transform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));

		//pPlayerObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, XM_PI));

		// MeshRender 설정
		//pPlayerObject->MeshRender()->SetDynamicShadow(true);
		//pPlayerObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		//pPlayerObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));
		//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
		//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());

		// Script 설정
		// 플레이어 일시
		if (i == m_playerId)
			m_pPlayerArray[i]->AddComponent(new CPlayerScript(m_pPlayerArray[i], true));
		else {
			m_pPlayerArray[i]->AddComponent(new CPlayerScript(m_pPlayerArray[i], false));
		}

		// AddGameObject
		//m_pCurScene->FindLayer(L"Player")->AddGameObject(m_pPlayerArray[i]);

	}
}
=======
>>>>>>> yjs
