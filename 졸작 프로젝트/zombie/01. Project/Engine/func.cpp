#pragma once
#include "stdafx.h"
#include "func.h"
#include "KeyMgr.h"
#include "TimeMgr.h"

#include "PathMgr.h"

#include "Core.h"
#include "Device.h"

#include "Mesh.h"
#include "shader.h"
#include "Texture.h"
#include "ConstantBuffer.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"

#include "KeyMgr.h"
#include "TimeMgr.h"

vector<VTX> g_vecVTX;
vector<UINT> g_vecIDX;

CMesh*				g_pMesh = nullptr;

CShader*			g_pShader = nullptr;
CMaterial*			g_pMtrl_0 = nullptr;
CMaterial*			g_pMtrl_1 = nullptr;

CTexture*			g_pTex = nullptr;

vector<CGameObject*> g_vecObj;

void TestInit()
{	
	g_pMesh = new CMesh;
	g_pShader = new CShader;
	
	VTX v;
	// 1. 입력 조립기 단계에 전달할, 정점 3개로 구성된 삼각형 1개
	v.vPos = Vec3(-0.5f, 0.5f, 0.5f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	g_vecVTX.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.5f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	g_vecVTX.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.5f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	g_vecVTX.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.5f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	g_vecVTX.push_back(v);

	g_vecIDX.push_back(0); g_vecIDX.push_back(1); g_vecIDX.push_back(2);
	g_vecIDX.push_back(0); g_vecIDX.push_back(2); g_vecIDX.push_back(3);
	
	g_pMesh->Create(sizeof(VTX), (UINT)g_vecVTX.size(), (BYTE*)g_vecVTX.data()
					, DXGI_FORMAT_R32_UINT, (UINT)g_vecIDX.size(), (BYTE*)g_vecIDX.data());
	   
	// 쉐이더 생성
	g_pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Test", "vs_5_0");
	g_pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Test", "ps_5_0");
	g_pShader->Create();
	
	// Texture 로드
	wstring strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Test.png";

	g_pTex = new CTexture;
	g_pTex->Load(strPath);

	// Material 생성
	int a = 1;
	int b = 2;
	g_pMtrl_0 = new CMaterial;
	g_pMtrl_0->SetShader(g_pShader);
	g_pMtrl_0->SetData(SHADER_PARAM::INT_0, &a);
	g_pMtrl_0->SetData(SHADER_PARAM::TEX_0, g_pTex);

	g_pMtrl_1 = new CMaterial;
	g_pMtrl_1->SetShader(g_pShader);
	g_pMtrl_1->SetData(SHADER_PARAM::INT_0, &b);
	

	// GameObject 만들기
	CGameObject* pObject = nullptr;
	{
		pObject = new CGameObject;
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->MeshRender()->SetMesh(g_pMesh);
		pObject->MeshRender()->SetMaterial(g_pMtrl_0);

		g_vecObj.push_back(pObject);

		pObject = new CGameObject;
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->MeshRender()->SetMesh(g_pMesh);
		pObject->MeshRender()->SetMaterial(g_pMtrl_1);

		g_vecObj.push_back(pObject);
	}
}

void TestUpdate()
{	
	for (UINT i = 0; i < g_vecObj.size(); ++i)
	{
		g_vecObj[i]->update();		
	}

	for (UINT i = 0; i < g_vecObj.size(); ++i)
	{
		g_vecObj[i]->lateupdate();
	}

	for (UINT i = 0; i < g_vecObj.size(); ++i)
	{
		g_vecObj[i]->finalupdate();
	}

	if (KEY_HOLD(KEY_TYPE::KEY_LEFT))
	{
		Vec3 vPos = g_vecObj[0]->Transform()->GetLocalPos();
		vPos.x -= DT * 1.f;
		g_vecObj[0]->Transform()->SetLocalPos(vPos);
	}

	if (KEY_HOLD(KEY_TYPE::KEY_RIGHT))
	{
		Vec3 vPos = g_vecObj[0]->Transform()->GetLocalPos();
		vPos.x += DT * 1.f;
		g_vecObj[0]->Transform()->SetLocalPos(vPos);
	}
}

void TestRender()
{
	// 그리기 준비
	float arrColor[4] = { 0.0f, 0.0f, 0.0f, 0.f };
		
	CDevice::GetInst()->render_start(arrColor);	
	
	for (size_t i = 0; i < g_vecObj.size(); ++i)
	{
		if (nullptr != g_vecObj[i]->MeshRender())
		{	
			g_vecObj[i]->MeshRender()->render();
		}			
	}

	// 그리기 종료
	CDevice::GetInst()->render_present();
}

void TestRelease()
{	
	SAFE_DELETE(g_pMesh);

	SAFE_DELETE(g_pMtrl_0);
	SAFE_DELETE(g_pMtrl_1);

	SAFE_DELETE(g_pShader);
	SAFE_DELETE(g_pTex);	
}

// ==============================


namespace RES_TYPE_STR
{
	const wchar_t* MATERIAL = L"MATERIAL";
	const wchar_t* MESH = L"MESH";
	const wchar_t* TEXTURE = L"TEXTURE";
	const wchar_t* SOUND = L"SOUND";
	const wchar_t* SHADER = L"SHADER";
};

const wchar_t* RES_TYPE_NAME[(UINT)RES_TYPE::END] =
{
	L"MATERIAL",
	L"MESH",
	L"TEXTURE",
	L"SOUND",
	L"SHADER",
};

int GetSizeofFormat(DXGI_FORMAT _eFormat)
{
	int iRetByte = 0;
	switch (_eFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		iRetByte = 128;
		break;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		iRetByte = 96;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		iRetByte = 64;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		iRetByte = 16;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		iRetByte = 8;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		iRetByte = 128;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		iRetByte = 64;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		iRetByte = 32;
		break;
		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return -1;
	}

	return iRetByte / 8;
}

void SaveWString(FILE * _pFile, const wstring & _str)
{
	BYTE c = (BYTE)_str.length();
	fwrite(&c, 1, 1, _pFile);
	fwrite(_str.c_str(), 2, c, _pFile);
}

wchar_t* LoadWString(FILE * _pFile)
{
	static wchar_t szStr[255] = {};

	BYTE c = 0;
	fread(&c, 1, 1, _pFile);
	fread(szStr, 2, c, _pFile);
	szStr[c] = 0;

	return szStr;
}
