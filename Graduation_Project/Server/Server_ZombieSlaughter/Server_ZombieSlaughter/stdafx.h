#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx�����ؼ�
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx�����ؼ�


#include <iostream>
#include <unordered_map>


using namespace std;



void err_display(char* msg);