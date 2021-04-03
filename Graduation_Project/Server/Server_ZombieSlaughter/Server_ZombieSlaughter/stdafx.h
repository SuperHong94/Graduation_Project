#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx를위해서
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx를위해서


#include <iostream>
#include <unordered_map>


using namespace std;



void err_display(char* msg);