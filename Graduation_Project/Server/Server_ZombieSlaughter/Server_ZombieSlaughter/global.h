#pragma once

#include <iostream>
#include <unordered_map>

//네트워크 관련
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx를위해서
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx를위해서
#include "Protocol.h"







#include <d3d12.h>
#pragma comment(lib, "d3d12")
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "SimpleMath.h"

#include "SimpleMath.h"

typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;
using DirectX::SimpleMath::Matrix;
using namespace std;