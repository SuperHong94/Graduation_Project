#include "stdafx.h"

#include "PlayerObject.h"


enum EOP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };
struct MY_OVER
{
	//send recv�Ҷ� ���� overlapped, ���۴� �������̾���Ѵ�.
	WSAOVERLAPPED m_over;
	WSABUF m_wsabuf[1];
	unsigned char m_buf[MAX_BUFFER];  //�� unsigned�̳ĸ� ���̳ʽ� ���� ���� �ʴ´�.
	EOP_TYPE m_eOP;//send���� recv���� �����Ѵ�.

};

struct CLIENT
{
	MY_OVER m_recv_over;
	SOCKET m_socket;
	int m_id;
	unsigned int m_prev_size;

	//�̺κ� �����ͷ� �ؾ��ұ� �ٸ������� �ؾ��ұ�?
	CPlayerObject* m_pPlayer;
};

constexpr int SERVER_ID = 0;
unordered_map<int, CLIENT> clients;





int get_new_id()
{
	for (int i = SERVER_ID + 1; i <= MAX_USER; ++i)
	{
		if (clients.count(i) == 0)return i;
	}
}




void send_packet(int c_id, void* packet)
{

	int bufSize = reinterpret_cast<unsigned char*>(packet)[0];
	int bufType = reinterpret_cast<unsigned char*>(packet)[1];
#ifdef _DEBUG
	std::cout << "send pacekt[" << bufType << "]" << " to [" << c_id << "]client\n";
#endif // _DEBUG

	MY_OVER* send_over = new MY_OVER;
	memset(&send_over->m_over, 0, sizeof(send_over->m_over));
	memcpy(send_over->m_buf, packet, bufSize);
	send_over->m_eOP = EOP_TYPE::OP_SEND;
	send_over->m_wsabuf[0].buf = reinterpret_cast<char*>(send_over->m_buf);
	send_over->m_wsabuf[0].len = bufSize;

	int retval = WSASend(clients[c_id].m_socket, send_over->m_wsabuf, 1, NULL, 0, &send_over->m_over, 0);
	if (retval != NO_ERROR)
	{
		int err_code = WSAGetLastError();
		if (err_code != WSA_IO_PENDING)
			err_display("WSASEND()", err_code);
	}
}
void send_move_packet(int c_id)
{
	CLIENT& client = clients[c_id];
	s2c_move packet;
	packet.size = sizeof(packet);
	packet.type = S2C_MOVE;
	Vec3 pos = client.m_pPlayer->GetPostion();
	packet.x = pos.x; packet.y = pos.y; packet.z = pos.z;
	Vec3 rot = client.m_pPlayer->GetRotation();
	packet.rx = rot.x; packet.ry = rot.y; packet.rz = rot.z;
	send_packet(c_id, &packet);
}
void send_login_result(int c_id)
{
	s2c_loginOK packet;
	packet.size = sizeof(packet);
	packet.type = S2C_LOGIN_OK;
	packet.id = c_id;
	packet.eScene_state = SCENE_STATE::START_SCENE;

	send_packet(c_id, &packet);
}
void send_scene_state(int c_id, c2s_chage_scene* packet)
{
	switch (packet->eSceneStatae)
	{
	case SCENE_STATE::START_SCENE:
	{
		//Ŭ��������� ��ŸƮ������ ����ȯ ��Ŷ�� ����°��
		//�������� �����ϴ� Ŭ���̾�Ʈ �����¸� inGame���·� �ٲ�
		clients[c_id].m_pPlayer->init();


		//
		for (auto& c : clients)
		{
			if (clients[c_id].m_pPlayer->GetSceneState() == SCENE_STATE::GAME_SCENE) { //game�������� Ŭ���̾�Ʈ���� addclient������
				send_add_client(c_id, c.second.m_id);//���������� Ŭ���̾�Ʈ���� �̹������� Ŭ���̾�Ʈ ����������
				send_add_client(c.second.m_id, c_id);//�̹������� Ŭ���̾�Ʈ�鿡�� ���������� Ŭ���̾�Ʈ ����������
			}
		}
		//���⿡ ���� �ʱ�ȭ ������ ��������
	}
	break;
	case SCENE_STATE::GAME_SCENE:
		break;
	case SCENE_STATE::GAMECLEAR_SCENE:
		break;
	case SCENE_STATE::GAMEOVER_SCENE:
		break;
	default:
		break;
	}

	s2c_chage_Scene p;
	p.size = sizeof(p);
	p.type = S2C_CHAGE_SCENE;
	p.eScene_state = clients[c_id].m_pPlayer->GetSceneState();

	send_packet(c_id, &p);

}
void send_key_result(int c_id, c2s_Key* packet)
{
	clients[c_id].m_pPlayer->Update(packet);

	send_move_packet(c_id);
}
/// <summary>
/// c_id�� other_id�� �߰��Ѵ�.
/// </summary>
/// <param name="c_id"> ��ü</param>
/// <param name="other_id"> ���</param>
void send_add_client(int c_id, int other_id)
{
	Vec3& otherRotation = clients[other_id].m_pPlayer->GetRotation();
	Vec3& otherPos = clients[other_id].m_pPlayer->GetPostion();
	s2c_add_client packet;
	packet.size = sizeof(packet);
	packet.type = S2C_ADD_PLAYER;
	packet.id = other_id;
	packet.rx = otherRotation.x; packet.ry = otherRotation.y; packet.rz = otherRotation.z;
	packet.x = otherPos.x; packet.y = otherPos.y; packet.z = otherPos.z;

	send_packet(c_id, &packet);
}
void proccess_packet(int c_id, unsigned char* buf)
{
	//buf[1]�� type�� ��
	switch (buf[1])
	{
	case C2S_LOGIN: //Ŭ�󿡼� �α��ο�û
	{
		c2s_login* packet = reinterpret_cast<c2s_login*>(buf);
		send_login_result(c_id);



	}
	break;
	case C2S_KEY_EVENT:
	{
		c2s_Key* packet = reinterpret_cast<c2s_Key*>(buf);
		send_key_result(c_id, packet);
	}
	case C2S_CHANGE_SCENE:
	{
		c2s_chage_scene* packet = reinterpret_cast<c2s_chage_scene*>(buf);
		send_scene_state(c_id, packet);
	}
	break;
	default:
		break;
	}
}
void do_recv(int c_id)
{
	clients[c_id].m_recv_over.m_wsabuf[0].buf = reinterpret_cast<char*>(clients[c_id].m_recv_over.m_buf) + clients[c_id].m_prev_size;
	clients[c_id].m_recv_over.m_wsabuf[0].len = MAX_BUFFER - clients[c_id].m_prev_size;
	memset(&clients[c_id].m_recv_over.m_over, 0, sizeof(clients[c_id].m_recv_over.m_over));
	DWORD rFlag = 0;
	int retval = WSARecv(clients[c_id].m_socket, clients[c_id].m_recv_over.m_wsabuf, 1, NULL, &rFlag, &clients[c_id].m_recv_over.m_over, 0);
	if (retval != NO_ERROR)
	{
		int err_code = WSAGetLastError();
		if (err_code != WSA_IO_PENDING)
			err_display("WSARecv()", err_code);
	}
}


void send_remove_client(int p_id, int other_id)
{
	s2c_remove_client packet;
	packet.size = sizeof(packet);
	packet.type = S2C_REMOVE_CLIENT;
	packet.m_id = other_id;
	send_packet(p_id, &packet);
}

void disconnect(int key)
{
	closesocket(clients[key].m_socket);
	delete clients[key].m_pPlayer;
	clients.erase(key);
	for (auto& c : clients)
		send_remove_client(c.second.m_id, key); //c.sencond.m_id���� key�� ����Ǿ����� �˸�
}

int main()
{
	wcout.imbue(locale("korean"));


	int ret = 0;
	WSADATA WSAdata;
	if (WSAStartup(MAKEWORD(2, 2), &WSAdata) != 0)
	{
		cout << "WSAStartUp ����" << endl;
		exit(-1); //�����Ű��
	}

	HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); //iocp�ڵ�

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == SOCKET_ERROR)
	{
		err_display("WSASocket()", WSAGetLastError());
		return 0;
	}
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, 0, 0);//iocp�� ���ϵ��

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = ::bind(listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		err_display("bind()", WSAGetLastError());
		return 0;
	}


	ret = listen(listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		err_display("listen()", WSAGetLastError());
		return 0;
	}


	MY_OVER accept_over; //accept�� overlapped ����ü
	accept_over.m_eOP = OP_ACCEPT; //�� overlapped�� �����뵵������ ��Ÿ����.
	memset(&accept_over.m_over, 0, sizeof(accept_over.m_over));

	//Ŭ���̾�Ʈ ����
	SOCKET c_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (c_sock == INVALID_SOCKET) {
		err_display("WSASocket()", WSAGetLastError());
		return 0;
	}
	BOOL b_ret = AcceptEx(listenSocket, c_sock, accept_over.m_buf, SERVER_ID, 32, 32, NULL, &accept_over.m_over);
	if (b_ret == FALSE)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			err_display("AcceptEx()", WSAGetLastError());
			while (true);
		}
	}

	while (true)
	{

		DWORD numBytes;
		ULONG_PTR ikey;
		WSAOVERLAPPED* over;
		b_ret = GetQueuedCompletionStatus(h_iocp, &numBytes, &ikey, &over, INFINITE);//�Ϸ�˻�

		int key = static_cast<int>(ikey);
		if (b_ret == FALSE)
		{
			if (key == SERVER_ID) {//��������
				err_display("GetQueuedCompletionStatus()", WSAGetLastError());
				cout << "��������\n";
				while (true);
				exit(-1);
			}
			else {
				err_display("GetQueuedCompletionStatus()", WSAGetLastError());
				cout << key << " client ��������\n";
				disconnect(key);
			}

		}
		MY_OVER* my_over = reinterpret_cast<MY_OVER*> (over);

		switch (my_over->m_eOP)
		{
		case OP_RECV:
		{
			//��Ŷ ����, ����
			unsigned char* packet_ptr = my_over->m_buf;
			int num_data = numBytes + clients[key].m_prev_size; //ó���ؾ��� �������� ũ��=���ݹ���������+�������� ������������ ������

			int packet_size = packet_ptr[0]; //���� ó���ؾ��ϴ� ��Ŷ�� ũ��



			while (packet_size <= num_data) {
				proccess_packet(key, packet_ptr);
				num_data -= packet_size;
				packet_ptr += packet_size;
				if (0 >= num_data)
					break;
				packet_size = packet_ptr[0];
			}
			clients[key].m_prev_size = num_data;//���� ��� ����
			if (0 != num_data)
				memcpy(my_over->m_buf, packet_ptr, sizeof(num_data));//�Ǿ����� ����

			do_recv(key);
		}
		break;
		case OP_SEND:
			delete my_over;
			break;
		case OP_ACCEPT:
		{


			int c_id = get_new_id();
			if (c_id != -1) {
#ifdef _DEBUG
				std::cout << "[TCP ����] " << c_id << "�� Ŭ���̾�Ʈ ����" << endl;
#endif // _DEBUG

				clients[c_id] = CLIENT{};
				clients[c_id].m_id = c_id;
				clients[c_id].m_socket = c_sock;
				clients[c_id].m_prev_size = 0;
				clients[c_id].m_recv_over.m_eOP = EOP_TYPE::OP_RECV;
				clients[c_id].m_pPlayer = new CPlayerObject;
				clients[c_id].m_pPlayer->init();
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_sock), h_iocp, c_id, 0);
				do_recv(c_id);
			}
			//��� Accept�ؾߵ�
			memset(&accept_over.m_over, 0, sizeof(accept_over.m_over));

			c_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			//AcceptEx()
			AcceptEx(listenSocket, c_sock, accept_over.m_buf, SERVER_ID, 32, 32, NULL, &accept_over.m_over);
		}
		break;
		default:
			break;
		}





	}
	closesocket(listenSocket);
	WSACleanup();



}
