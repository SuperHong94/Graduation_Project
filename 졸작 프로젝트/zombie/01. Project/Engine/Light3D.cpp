#include "stdafx.h"
#include "Light3D.h"

#include "Transform.h"
#include "RenderMgr.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
{
}

CLight3D::~CLight3D()
{
}

void CLight3D::SetLightType(LIGHT_TYPE _eType)
{
	m_tLightInfo.iLightType = (int)_eType;

}

void CLight3D::SetLightPos(const Vec3 & _vPos)
{
	m_tLightInfo.vLightPos = _vPos;
	Transform()->SetLocalPos(_vPos);
}

void CLight3D::SetLightDir(const Vec3& _vDir)
{
	m_tLightInfo.vLightDir = _vDir;
	m_tLightInfo.vLightDir.Normalize();
	// Transform()->LookAt(_vDir);
}

void CLight3D::finalupdate()
{
	m_tLightInfo.vLightPos = Transform()->GetWorldPos();
	CRenderMgr::GetInst()->RegisterLight3D(this);
}

void CLight3D::SaveToScene(FILE * _pFile)
{
}

void CLight3D::LoadFromScene(FILE * _pFile)
{
}