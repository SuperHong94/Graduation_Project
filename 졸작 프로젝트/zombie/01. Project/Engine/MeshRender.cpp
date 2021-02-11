#include "stdafx.h"
#include "MeshRender.h"

#include "Transform.h"
#include "ResMgr.h"


CMeshRender::CMeshRender()
	: CComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}


void CMeshRender::render()
{
	if (IsActive() == false || nullptr == m_pMesh || nullptr == m_pMtrl || nullptr == m_pMtrl->GetShader())
		return;

	Transform()->UpdateData();

	m_pMtrl->UpdateData();
	m_pMesh->render();
}

Ptr<CMaterial> CMeshRender::GetCloneMaterial()
{
	if (nullptr == m_pMtrl)
		return nullptr;

	m_pMtrl = m_pMtrl->Clone();
	return m_pMtrl;	
}

void CMeshRender::SaveToScene(FILE * _pFile)
{
	UINT iType = (UINT)GetComponentType();
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	// 존재여부 저장(nullptr 인지 아닌지)
	fwrite(&m_pMesh, sizeof(void*), 1, _pFile);

	if (nullptr != m_pMesh)
	{
		SaveWString(_pFile, m_pMesh->GetName());
		SaveWString(_pFile, m_pMesh->GetPath());
	}

	// 존재여부 저장(nullptr 인지 아닌지)
	fwrite(&m_pMtrl, sizeof(void*), 1, _pFile);

	if (nullptr != m_pMtrl)
	{
		SaveWString(_pFile, m_pMtrl->GetName());
		SaveWString(_pFile, m_pMtrl->GetPath());
	}
}

void CMeshRender::LoadFromScene(FILE * _pFile)
{
	void* pData = nullptr;
	fread(&pData, sizeof(void*), 1, _pFile);

	if (pData)
	{
		wstring strMesh = LoadWString(_pFile);
		wstring strPath = LoadWString(_pFile);
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(strMesh);

		if (nullptr == m_pMesh)
		{
			m_pMesh = CResMgr::GetInst()->Load<CMesh>(strMesh, strPath);
		}
	}

	fread(&pData, sizeof(void*), 1, _pFile);

	if (pData)
	{
		wstring strMtrl = LoadWString(_pFile);
		wstring strPath = LoadWString(_pFile);
		m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strMtrl);

		if (nullptr == m_pMtrl)
		{
			m_pMtrl = CResMgr::GetInst()->Load<CMaterial>(strMtrl, strPath);
		}
	}
}