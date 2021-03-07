#include "stdafx.h"
#include "MeshRender.h"

#include "Transform.h"
#include "ResMgr.h"


CMeshRender::CMeshRender()
	: CComponent(COMPONENT_TYPE::MESHRENDER)
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

	for (size_t i = 0; i < m_vecMtrl.size(); ++i)
	{
		if (nullptr == m_vecMtrl[i] || nullptr == m_vecMtrl[i]->GetShader())
			continue;

		Transform()->UpdateData();
		m_vecMtrl[i]->UpdateData();
		m_pMesh->render((UINT)i);
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

void CMeshRender::SaveToScene(FILE * _pFile)
{	
}

void CMeshRender::LoadFromScene(FILE * _pFile)
{
}