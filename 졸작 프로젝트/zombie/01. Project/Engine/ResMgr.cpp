#include "stdafx.h"
#include "ResMgr.h"

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Sound.h"

//#include "RenderMgr.h"


CResMgr::CResMgr()
{
}

CResMgr::~CResMgr()
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Vector(m_vecCloneRes[i]);
	}

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Map(m_mapRes[i]);
	}
}

void CResMgr::init()
{
	CreateDefaultMesh();

	CreateDefaultShader();

	CreateDefaultMaterial();

	// FMOD 초기화
	InitSound();
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strName, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
	, const D3D12_HEAP_PROPERTIES & _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
	, D3D12_RESOURCE_FLAGS _eResFlag)
{
	Ptr<CTexture> pNewTexture = new CTexture;
	pNewTexture->Create(_iWidth, _iHeight, _eFormat, _HeapProperty, _eHeapFlag, _eResFlag);	
	AddRes<CTexture>(_strName, pNewTexture);

	return pNewTexture;
}

Ptr<CTexture> CResMgr::CreateTextureFromResource(const wstring & _strName, ComPtr<ID3D12Resource> _pTex2D)
{
	Ptr<CTexture> pNewTexture = new CTexture;
	pNewTexture->CreateFromResource(_pTex2D);	
	AddRes<CTexture>(_strName, pNewTexture);

	return pNewTexture;
}

//Ptr<CTexture> CResMgr::CreateTexture(const wstring _strKey, float _fWidth, float _iHeight
//	, UINT _iBindFlag, DXGI_FORMAT _eFormat, D3D11_USAGE _eUsage)
//{
//	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
//
//	if (nullptr != pTex)
//		assert(nullptr);
//
//	pTex = new CTexture;	
//	if (!pTex->Create(_fWidth, _iHeight, _iBindFlag, _eFormat, _eUsage))
//	{
//		MessageBox(nullptr, _strKey.c_str(), L"텍스쳐 생성 실패", MB_OK);
//		assert(nullptr);
//	}
//
//	pTex->SetName(_strKey);
//
//	m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.GetPointer()));
//
//	return pTex;
//}
//
//Ptr<CTexture> CResMgr::CreateTexture(const wstring & _strKey, ID3D11Texture2D * _pTex2D)
//{
//	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
//
//	if (nullptr != pTex)
//		assert(nullptr);
//
//	pTex = new CTexture;
//	if (!pTex->Create(_pTex2D))
//	{
//		MessageBox(nullptr, _strKey.c_str(), L"텍스쳐 생성 실패", MB_OK);
//		assert(nullptr);
//	}
//
//	pTex->SetName(_strKey);
//
//	m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.GetPointer()));
//
//	return pTex;
//}

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL *channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void *commanddata1, void *commanddata2);

void CResMgr::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);	
}
