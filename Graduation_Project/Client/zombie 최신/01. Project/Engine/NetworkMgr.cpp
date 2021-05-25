#include "stdafx.h"
#include "NetworkMgr.h"
#include "GameObject.h"
#include "Transform.h" //이동을위한 헤더
#include "TimeMgr.h" //DT를 위해서
#include "PlayerScript.h"	

#include "SceneMgr.h"

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
	unsigned char type = buf[1];
	unsigned char s = buf[0];
	static DWORD inPacketSize = 0; //새패킷
	//DWORD inPacketSize = 0; //새패킷
	static DWORD savedPacketSize = 0; //잔여패킷
	static char packetBuffer[MAX_BUFFER];
	while (bufsize != 0)
	{
		if (inPacketSize == 0)
			inPacketSize = ptr[0]; //여기때문에 안된거임
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

void CNetworkMgr::process(char* buf)
{

	unsigned char a = buf[1];

	switch (buf[1])//타입확인
	{
	case S2C_LOGIN_OK:
	{
		s2c_loginOK* p = reinterpret_cast<s2c_loginOK*>(buf);



		m_id = p->id; //이것은 통신하기위한 ID
		m_playerId = m_id - 1; //이것은 그리기위한 ID 이렇게 한이유는 m_id의 0은 서버ID이다. 하지만 랜더링을 위한 플레이어 배열의 시작은 0부터시작하기에 이렇게 하였다.



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

	case S2C_ADD_PLAYER: //추가접속플레이어
	{
#ifdef _DEBUG
		std::cout << "add Player\n";
#endif // _DEBUG

		s2c_add_client* packet = reinterpret_cast<s2c_add_client*>(buf);
		int id = packet->id - 1;
		if (m_pPlayerArray != nullptr) {


			m_pPlayerArray[id]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = false;
			m_pPlayerArray[id]->GetScript<CPlayerScript>()->Transform()->SetLocalPos(Vec3(packet->x, packet->y, packet->z));

		}
	}
	break;
	case S2C_MOVE:
	{

		s2c_move* p = reinterpret_cast<s2c_move*>(buf);
		process_key(p);
	}
	break;
	case S2C_REMOVE_CLIENT: //클라이언트 접속 종료된
	{
		s2c_remove_client* packet = reinterpret_cast<s2c_remove_client*>(buf);
		int id = packet->id - 1;
		if (m_pPlayerArray != nullptr) {

			m_pPlayerArray[id]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = true;
			m_pPlayerArray[id]->GetScript<CPlayerScript>()->Transform()->SetLocalPos(Vec3(20000.f, 20000.f, 20000.f));

		}
	}
	break;
	case S2C_DUMMY:
	{
#ifdef _DEBUG
		std::cout << "더미 받음\n";
#endif
	}
	default:
#ifdef _DEBUG
		std::cout << "알 수 없는 패킷 받음\n";
#endif
		break;
	}
}

void CNetworkMgr::process_key(s2c_move* p)
{

	int playerID = p->id - 1;
	if (m_pPlayerArray != nullptr) {
		//m_pPlayerArray[playerID]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = false;

		const auto& PlayerScript = m_pPlayerArray[playerID]->GetScript<CPlayerScript>();

		auto& state = PlayerScript->GetStatus()->state;
		Vec3 prevPos = PlayerScript->Transform()->GetLocalPos(); //이전 좌표
		state = p->ePlayerState;
		if (state == PlayerState::P_BRun || state == PlayerState::P_FRun || state == PlayerState::P_LRun || state == PlayerState::P_RRun) {
			PlayerScript->Transform()->SetLocalPos(Vec3(p->x, 53.f, p->z));
			PlayerScript->GetStatus()->isMove = true;
		}
		else{
			PlayerScript->Transform()->SetLocalPos(Vec3(p->x, 0.f, p->z));
			PlayerScript->GetStatus()->isMove = false;
		}
		if (playerID != m_playerId) { //내가 조정하는거 아닐때만
			PlayerScript->Transform()->SetLocalRot(Vec3(p->rx, p->ry, p->rz));
			/*if ((prevPos.x == p->x) && (prevPos.y == p->y))
				PlayerScript->GetStatus()->isMove = false;*/
			//PlayerScript->Transform()->SetLocalPos(Vec3(p->x, 0.f, p->z));

		}
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


void CNetworkMgr::send_chage_scene()
{
	c2s_chage_scene packet;
	packet.size = sizeof(packet);
	packet.type = C2S_CHANGE_SCENE;
	packet.id = m_id;
	packet.eSceneStatae = m_eSceneState;
	send_packet(&packet);
}




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
	packet.isMove = true;
#ifdef _DEBUG
	std::cout << "서버에게" << key << " 키 정보를 보낸다.\n";
#endif // _DEBUG

	send_packet(&packet);
}







void CNetworkMgr::init()
{


	//m_pPlayerArray = nullptr;
	//for (int i = 0; i < MAX_USER; ++i) {
	//	m_pPlayerArray[i] = nullptr;
	//}


	std::wcout.imbue(std::locale("korean"));
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


	int retval = 0;
	if (retval != NO_ERROR)
	{
		err_display("ioctlsocket", WSAGetLastError());
		exit(-1);
	}
	//connect()
	char ip[30];
	::memset(ip, 0, sizeof(ip));
	ifstream in("IP.txt");
	if (!in) {
		exit(-1);
		cout << "안됨\n";
	}
	in >> ip;
	SOCKADDR_IN serveraddr;
	::memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, ip, &serveraddr.sin_addr);
	retval = connect(m_sock, reinterpret_cast<SOCKADDR*>(&serveraddr), sizeof(serveraddr));

	//논블록 소켓으로 설정
	unsigned long noblock = 1;
	retval = ioctlsocket(m_sock, FIONBIO, &noblock);
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
#ifdef _DEBUG
	std::cout << "플레이어 초기화 완료\n";
#endif // _DEBUG

	}
