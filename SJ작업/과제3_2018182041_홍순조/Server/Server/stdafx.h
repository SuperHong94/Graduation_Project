#pragma once
#include <iostream>
#include <unordered_map>
#include <WS2tcpip.h> 
#pragma comment(lib, "ws2_32")
//#include <ws2tcpip.h>


using namespace std;




// ���� �Լ� ���� ��� �� ����
//void err_quit(char* msg);

// ���� �Լ� ���� ���
void err_display(char* msg);
void display_error(const char* msg, int err_no);