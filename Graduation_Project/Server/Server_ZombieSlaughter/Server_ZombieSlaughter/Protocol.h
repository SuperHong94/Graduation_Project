#pragma once

#define SERVER_IP "127.0.0.1"
constexpr int MAX_USER = 10;
constexpr int MAX_NAME = 100;
constexpr int MAX_BUFFER = 1024;
constexpr short  SERVER_PORT = 3500;

constexpr unsigned char C2S_LOGIN = 1;
constexpr unsigned char C2S_KEY_EVENT = 2;
constexpr unsigned char S2C_LOGIN_OK = 3;
constexpr unsigned char S2C_ADD_PLAYER = 4;


constexpr unsigned char S2C_MOVE = 5;

constexpr unsigned char S2C_REMOVE_CLIENT= 6;

enum EKEY_EVENT
{
	DOWN_UP,DOWN_LEFT,DOWN_RIGHT,DOWN_LEFT
};

#pragma pack(push,1)
//client->Serve
struct c2s_login
{
	unsigned char size;
	unsigned char type;
};


struct c2s_Key
{
	unsigned char size;
	unsigned char type;
	EKEY_EVENT key;
};
//////////////////
//Server->Client
struct s2c_move
{
	unsigned char size;
	unsigned char type;
	float x, y, z;
};

struct s2c_loginOK
{
	unsigned char size;
	unsigned char type;
	float x, y, z;  //초기위치

};
#pragma pack(pop)