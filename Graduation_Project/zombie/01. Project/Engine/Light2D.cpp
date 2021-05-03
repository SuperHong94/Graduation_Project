#include "stdafx.h"
#include "Light2D.h"

#include "RenderMgr.h"
#include "Transform.h"


CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
	, m_LightInfo{}
{
	m_LightInfo.iLightType = (UINT)LIGHT_TYPE::END;
}

CLight2D::~CLight2D()
{
}

void CLight2D::finalupdate()
{
	if (!IsActive())
		return;

	m_LightInfo.vLightPos = Transform()->GetWorldPos();

	// rendermgr ¿¡ µî·Ï
	CRenderMgr::GetInst()->RegisterLight2D(m_LightInfo);
}

void CLight2D::UpdateData()
{
}


void CLight2D::SaveToScene(FILE * _pFile)
{
	UINT iType = (UINT)GetComponentType();
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_LightInfo, sizeof(tLight2D), 1, _pFile);	
}

void CLight2D::LoadFromScene(FILE * _pFile)
{
	fread(&m_LightInfo, sizeof(tLight2D), 1, _pFile);
}