#pragma once

#define SERVER_IP "127.0.0.1"


constexpr int MAX_USER = 10;
constexpr int MAX_NAME = 100;
constexpr int MAX_BUFFER = 1024;

//클라크기
constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;
//맵 크기
constexpr int MAX_MAP_X = 4990;
constexpr int MIN_MAP_X = -4990;
constexpr int MAX_MAP_Z = 4990;
constexpr int MIN_MAP_Z = -4990;


constexpr unsigned short MAX_BULLET = 200;
constexpr short  SERVER_PORT = 3500;

constexpr unsigned char C2S_LOGIN = 1;
constexpr unsigned char C2S_KEY_EVENT = 2;
constexpr unsigned char S2C_LOGIN_OK = 3;
constexpr unsigned char S2C_ADD_PLAYER = 4;


constexpr unsigned char S2C_MOVE = 5;

constexpr unsigned char S2C_REMOVE_CLIENT= 6;

constexpr unsigned char C2S_MOUSE_EVENT= 7;

enum EKEY_EVENT
{
	DOWN_UP=0,DOWN_DOWN,DOWN_RIGHT,DOWN_LEFT,DOWN_LB,NO_EVENT
};

enum EPlayerState
{
	eP_Idle,
	eP_FRun,
	eP_BRun,
	eP_LRun,
	eP_RRun,
	eP_Attack,
	eP_Die,
	eP_Spawn,
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
	float dT; //deltatime
	float rX, rY, rZ;// 회전정보
	unsigned char isMove; //0이면 안움직임 1이면 움직임
};


//////////////////
//Server->Client
struct s2c_move
{
	unsigned char size;
	unsigned char type;
	float x, y, z;
	float rx, ry, rz;
	//POINT mousePos;
};

struct s2c_loginOK
{
	unsigned char size;
	unsigned char type;
	float x, y, z;  //초기위치

};

struct s2c_remove_client
{
	unsigned char size;
	unsigned char type;
	int m_id;
};
#pragma pack(pop)