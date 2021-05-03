#pragma once
#include <iostream>
#include <unordered_map>
#include <WS2tcpip.h> 
#pragma comment(lib, "ws2_32")
//#include <ws2tcpip.h>


using namespace std;




// 소켓 함수 오류 출력 후 종료
//void err_quit(char* msg);

// 소켓 함수 오류 출력
void err_display(char* msg);
void display_error(const char* msg, int err_no);