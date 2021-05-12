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
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

#include <string>
#include <vector>
#include <list>
#include <map>


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


// 서버관련 lib, 헤더추가
#include <WS2tcpip.h>
#include <MSWSock.h>//acceptEx를위해서
#include <fstream>	
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib") //acceptEx를위해서
#include "../../../../Server/Server_ZombieSlaughter/Server_ZombieSlaughter/Protocol.h"

#include <iostream>
#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") //유니코드


#endif

#define FRAME_BUFFER_WIDTH		1280
#define FRAME_BUFFER_HEIGHT		768