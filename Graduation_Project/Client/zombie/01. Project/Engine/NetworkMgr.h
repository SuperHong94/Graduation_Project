#pragma once
class CNetworkMgr
{
	SINGLE(CNetworkMgr);
private:
	WSADATA m_wsa;
	SOCKET m_sock;

public:
	void init();
	void err_display(const char* msg, int error);
};


