#pragma once


// simd 사용 안함
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
//콘솔창 나오게 하기
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif




#include <string>
#include <vector>
#include <list>
#include <map>

//서버 관련 헤더, lib////////////////
#include <WS2tcpip.h>
#include <iostream> //서버관련 콘솔창에 출력하기
#include "..\..\..\..\Server\Server_ZombieSlaughter\Server_ZombieSlaughter\Protocol.h" //프로토콜

#pragma comment(lib, "ws2_32")


/////////////////////////////////
using std::vector;
using std::list;
using std::map;
using std::string;
using std::wstring;
using std::make_pair;

#include "d3dx12.h"	// Dx12 서포트 헤더
#include "define.h" // 매크로, enum
#include "struct.h" // 구조체 설계
#include "extern.h" // extern 변수
#include "func.h"   // 전역 함수

