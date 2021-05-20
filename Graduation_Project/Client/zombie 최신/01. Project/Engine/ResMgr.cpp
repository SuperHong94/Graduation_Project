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
	, D3D12_RESOURCE_FLAGS _eResFlag, Vec4 _vClearColor)
{
	Ptr<CTexture> pNewTexture = new CTexture;
	pNewTexture->Create(_iWidth, _iHeight, _eFormat, _HeapProperty, _eHeapFlag, _eResFlag, _vClearColor);
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

Ptr<CMeshData> CResMgr::LoadFBX(const wstring & _strPath)
{
	wstring strFileName = CPathMgr::GetFileName(_strPath.c_str());

	wstring strName = L"MeshData\\";
	strName += strFileName + L".mdat";

	Ptr<CMeshData> pMeshData = FindRes<CMeshData>(strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(_strPath);
	pMeshData->SetName(strName);
	pMeshData->SetPath(strName);
	m_mapRes[(UINT)RES_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.GetPointer()));

	return pMeshData;
}

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
