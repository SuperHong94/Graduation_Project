#pragma once

#include <iostream>
#include <unordered_map>

//��Ʈ��ũ ����
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx�����ؼ�
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx�����ؼ�
#include "Protocol.h"


//����
#include "SimpleMath.h"


typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;
using DirectX::SimpleMath::Matrix;
using namespace std;




