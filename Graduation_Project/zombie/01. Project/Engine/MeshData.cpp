#include "stdafx.h"
#include "MeshData.h"

#include "PathMgr.h"
#include "Mesh.h"
#include "ResMgr.h"
#include "Material.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator3D.h"
#include "GameObject.h"

CMeshData::CMeshData()
	: CResource(RES_TYPE::MESHDATA)
{
}

CMeshData::~CMeshData()
{
}

CMeshData * CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = CMesh::CreateFromContainer(loader);

	// Animation 이 있는 Mesh 경우 BoneTexture 만들어두기
	if (pMesh->IsAnimMesh())
	{
		wstring strBoneTex = _strPath;
		strBoneTex += L"BoneTex";
	
		Ptr<CTexture> pBoneTex =
			CResMgr::GetInst()->CreateTexture(strBoneTex
				, (pMesh->GetBones()->size() * 4), 1
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
				, D3D12_HEAP_FLAG_NONE
				, D3D12_RESOURCE_FLAG_NONE);
	
		pMesh->SetBoneTex(pBoneTex);
	}	

	// ResMgr 에 메쉬 등록

	wstring strMeshName = L"Mesh\\";
	strMeshName += CPathMgr::GetFileName(strFullPath.c_str());
	strMeshName += L".mesh";

	pMesh->SetName(strMeshName);
	pMesh->SetPath(strMeshName);

	CResMgr::GetInst()->AddRes<CMesh>(pMesh->GetName(), pMesh);

	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}


void CMeshData::Load(const wstring & _strFilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Mesh Load
	wstring strMeshKey, strMeshPath;
	strMeshKey = LoadWString(pFile);
	strMeshPath = LoadWString(pFile);
	m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMeshKey, strMeshPath);

	// material 정보 읽기
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, pFile);
		
	m_vecMtrl.resize(iMtrlCount);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		UINT idx = -1;
		fread(&idx, 4, 1, pFile);
		if (idx == -1)
			break;
		
		wstring strKey = LoadWString(pFile);
		wstring strPath = LoadWString(pFile);

		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey, strPath);
		m_vecMtrl[i] = pMtrl;
	}

	fclose(pFile);
}

void CMeshData::Save(const wstring& _strFilePath)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += _strFilePath;
	SetPath(_strFilePath);

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	// Mesh 를 파일로 저장
	m_pMesh->Save(m_pMesh->GetPath());
	
	// Mesh Key 저장	
	// Mesh Data 저장
	SaveWString(pFile, m_pMesh->GetName().c_str());
	SaveWString(pFile, m_pMesh->GetPath().c_str());

	// material 정보 저장
	UINT iMtrlCount = m_pMesh->GetSubsetCount();
	iMtrlCount = (UINT)m_vecMtrl.size();	
	fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);

	UINT i = 0;
	for (; i < iMtrlCount; ++i)
	{
		if (nullptr == m_vecMtrl[i])
			continue;

		// Material 을 파일로 저장
		m_vecMtrl[i]->Save(m_vecMtrl[i]->GetPath());

		// Material 인덱스, Key, Path 저장
		fwrite(&i, sizeof(UINT), 1, pFile);
		SaveWString(pFile, m_vecMtrl[i]->GetName().c_str());
		SaveWString(pFile, m_vecMtrl[i]->GetPath().c_str());
	}

	i = -1; // 마감 값
	fwrite(&i, sizeof(UINT), 1, pFile);

	fclose(pFile);
}

CGameObject * CMeshData::Instantiate()
{
	// Mesh
	// Material

	CGameObject* pNewObj = new CGameObject;	
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);
	
	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	if (false == m_pMesh->IsAnimMesh())
		return pNewObj;	
	
	CAnimator3D* pAnimator = new CAnimator3D;
	pNewObj->AddComponent(pAnimator);
	
	pAnimator->SetBones(m_pMesh->GetBones());
	pAnimator->SetAnimClip(m_pMesh->GetAnimClip());
	pAnimator->SetBoneTex(m_pMesh->GetBoneTex());

	return pNewObj;
}

