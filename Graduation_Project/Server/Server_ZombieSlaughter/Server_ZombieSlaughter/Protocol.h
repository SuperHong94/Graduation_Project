#pragma once

#define SERVER_IP "127.0.0.1"


constexpr int MAX_USER = 4; //0번은 서버


constexpr int MAX_NAME = 100;
constexpr int MAX_BUFFER = 1024;


enum PlayerState
{
	P_Idle,
	P_FRun,
	P_BRun,
	P_LRun,
	P_RRun,
	P_Attack,
	P_Die,
	P_Spawn,
	P_Roll,
	p_None,
};

//클라크기
constexpr int FRAME_BUFFER_WIDTH = 1024;
constexpr int FRAME_BUFFER_HEIGHT = 768;
//맵 크기
constexpr int MAX_MAP = 4990;
constexpr int MIN_MAP = -4990;



constexpr unsigned short MAX_BULLET = 200;
constexpr short  SERVER_PORT = 3500;

constexpr unsigned char C2S_LOGIN = 1;
constexpr unsigned char C2S_KEY_EVENT = 2;
constexpr unsigned char S2C_LOGIN_OK = 3;
constexpr unsigned char S2C_ADD_PLAYER = 4;


constexpr unsigned char S2C_MOVE = 5;

constexpr unsigned char S2C_REMOVE_CLIENT= 6;


constexpr unsigned char C2S_MOUSE_EVENT= 7;
constexpr unsigned char C2S_CHANGE_SCENE= 8;
constexpr unsigned char S2C_CHAGE_SCENE = 9;
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

enum class SCENE_STATE
{
	START_SCENE,
	GAME_SCENE,
	GAMECLEAR_SCENE,
	GAMEOVER_SCENE,
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

struct c2s_chage_scene
{

	unsigned char size;
	unsigned char type;
	int id; //누가 보냈는지
	SCENE_STATE eSceneStatae; //클라이언트의 현재씬

};

//////////////////
//Server->Client
struct s2c_move
{
	unsigned char size;
	unsigned char type;
	PlayerState ePlayerState;
	float x, y, z;
	float rx, ry, rz;
	//POINT mousePos;
};

struct s2c_loginOK
{
	unsigned char size;
	unsigned char type;
	int id;
	SCENE_STATE eScene_state;

};

struct s2c_remove_client
{
	unsigned char size;
	unsigned char type;
	int m_id;
	
};

struct s2c_chage_Scene
{
	unsigned char size;
	unsigned char type;
	int id; //바꾸는 씬 정보
	SCENE_STATE eScene_state;

};
struct s2c_add_client
{
	unsigned char size;
	unsigned char type;
	int id;//추가할 클라ID
	float x, y, z; //추가할 클라의 좌표값
	float rx, ry, rz; //추가할 클라의 회전값
};
#pragma pack(pop)