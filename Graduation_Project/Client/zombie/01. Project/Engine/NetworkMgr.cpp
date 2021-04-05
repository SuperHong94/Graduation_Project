#include "stdafx.h"
#include "NetworkMgr.h"
#include "..\..\..\..\Server\Server_ZombieSlaughter\Server_ZombieSlaughter\Protocol.h"



//������ 
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
	while (true);//������ؾ���
}
void CNetworkMgr::init()
{
	using namespace std;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		cout << "WSAStartUp ����" << endl;
		exit(-1); //�����Ű��
	}

	m_sock =WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, NULL);
	if (m_sock == INVALID_SOCKET)
	{
		err_display("WSASocket()", WSAGetLastError());
		exit(-1);
	}

	//connect()
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr);
	int retval = connect(m_sock, reinterpret_cast<SOCKADDR*>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("connect()", WSAGetLastError());
		exit(-1);
	}
	else
#ifdef _DEBUG
		cout << "������ connect ����" << endl;
#endif // _DEBUG

}
