#pragma once



constexpr unsigned char C2S_LOGIN = 1;
constexpr unsigned char C2S_KEY_EVENT = 2;
constexpr unsigned char S2C_LOGIN_RESULT = 3;
constexpr unsigned char S2C_ADD_PLAYER = 4;


constexpr unsigned char S2C_MOVE = 5;

constexpr unsigned char S2C_REMOVE_CLIENT= 6;

#pragma pack(push,1)
struct c2s_Key
{
	unsigned char size;
	unsigned char type;
	char key;
};

struct s2c_move
{
	unsigned char size;
	unsigned char type;
	float x, y, z;
};

#pragma pack(pop)