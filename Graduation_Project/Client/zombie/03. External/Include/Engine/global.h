#pragma once


// simd ��� ����
#define _XM_NO_INTRINSICS_

#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <algorithm>
#include "SimpleMath.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;


// Fbx Loader
#include "fbxsdk.h"

#ifdef _DEBUG
//�ܼ�â ������ �ϱ�
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif




#include <string>
#include <vector>
#include <list>
#include <map>

//���� ���� ���, lib////////////////
#include <WS2tcpip.h>
#include <iostream> //�������� �ܼ�â�� ����ϱ�
#include "..\..\..\..\Server\Server_ZombieSlaughter\Server_ZombieSlaughter\Protocol.h" //��������

#pragma comment(lib, "ws2_32")


/////////////////////////////////
using std::vector;
using std::list;
using std::map;
using std::string;
using std::wstring;
using std::make_pair;

#include "d3dx12.h"	// Dx12 ����Ʈ ���
#include "define.h" // ��ũ��, enum
#include "struct.h" // ����ü ����
#include "extern.h" // extern ����
#include "func.h"   // ���� �Լ�

