#include "stdafx.h"
#include "MeshRender.h"

#include "Transform.h"


CMeshRender::CMeshRender()
	: CComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::render()
{
	Transform()->UpdateData();

	m_pShader->UpdateData();
	m_pMesh->render();
}
