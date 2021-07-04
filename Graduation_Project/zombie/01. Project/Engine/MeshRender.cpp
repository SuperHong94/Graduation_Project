#include "stdafx.h"
#include "MeshRender.h"

#include "Transform.h"
#include "ResMgr.h"
#include "Animator3D.h"

CMeshRender::CMeshRender()
	: CComponent(COMPONENT_TYPE::MESHRENDER)
	, m_bDynamicShadow(false)
{
	m_vecMtrl.resize(1);
}

CMeshRender::~CMeshRender()
{
}


void CMeshRender::render()
{
	if (IsActive() == false || nullptr == m_pMesh)
		return;

	int a = 1;
	for (size_t i = 0; i < m_vecMtrl.size(); ++i)
	{
		if (nullptr == m_vecMtrl[i] || nullptr == m_vecMtrl[i]->GetShader())
			continue;

		// Transform 정보 업데이트
		Transform()->UpdateData();

		// Animator3D 컴포넌트가 있는 경우...
		if (Animator3D())
		{
			Animator3D()->UpdateData();
			a = 1;
			m_vecMtrl[i]->SetData(SHADER_PARAM::INT_0, &a); // Animation Mesh 알리기
		}

		m_vecMtrl[i]->UpdateData();
		m_pMesh->render((UINT)i);

		a = 0;
		m_vecMtrl[i]->SetData(SHADER_PARAM::INT_0, &a); // Animation Mesh 알리기
	}
}

void CMeshRender::render_shadowmap()
{
	int a = 1;
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	for (UINT i = 0; i < m_pMesh->GetSubsetCount(); ++i)
	{
		if (Animator3D())
		{
			Animator3D()->UpdateData();
			pMtrl->SetData(SHADER_PARAM::INT_0, &a); // Animation Mesh 알리기
		}

		Transform()->UpdateData();
		pMtrl->UpdateData();
		m_pMesh->render(i);
	}

	// 정리
	if (Animator3D())
	{
		a = 0;
		pMtrl->SetData(SHADER_PARAM::INT_0, &a);
	}
}

Ptr<CMaterial> CMeshRender::GetCloneMaterial(UINT _iSubSet)
{
	if (nullptr == m_vecMtrl[_iSubSet])
		return nullptr;

	m_vecMtrl[_iSubSet] = m_vecMtrl[_iSubSet]->Clone();
	return m_vecMtrl[_iSubSet];
}

void CMeshRender::SetMaterial(Ptr<CMaterial> _pMtrl, UINT _iSubset)
{
	if (m_vecMtrl.size() <= (size_t)_iSubset)
		m_vecMtrl.resize(_iSubset + 1);

	m_vecMtrl[_iSubset] = _pMtrl;
}

void CMeshRender::SaveToScene(FILE* _pFile)
{
}

void CMeshRender::LoadFromScene(FILE* _pFile)
{
}