#include "stdafx.h"
#include "Light3D.h"

#include "Transform.h"
#include "RenderMgr.h"

#include "ResMgr.h"

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

	if (LIGHT_TYPE::DIR == (LIGHT_TYPE)m_tLightInfo.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	}
	else if (LIGHT_TYPE::POINT == (LIGHT_TYPE)m_tLightInfo.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	}
	else if (LIGHT_TYPE::SPOT == (LIGHT_TYPE)m_tLightInfo.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
	}
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
	Transform()->SetLocalScale(Vec3(m_tLightInfo.fRange, m_tLightInfo.fRange, m_tLightInfo.fRange));
	m_iArrIdx = CRenderMgr::GetInst()->RegisterLight3D(this);
}

void CLight3D::render()
{
	if (-1 == m_iArrIdx)
		return;

	Transform()->UpdateData();
	m_pLightMtrl->SetData(SHADER_PARAM::INT_0, &m_iArrIdx); // ±¤¿ø ¹è¿­ ÀÎµ¦½ºÁ¤º¸ ¾÷µ¥ÀÌÆ®
	m_pLightMtrl->UpdateData(); // int_0, ±¤¿ø ÀÎµ¦½º , tex_0 : normaltarget, tex_1 : positiontarget 
	m_pVolumeMesh->render();
}

void CLight3D::SaveToScene(FILE * _pFile)
{
}

void CLight3D::LoadFromScene(FILE * _pFile)
{
}