#include "stdafx.h"
#include "Collider2D.h"

#include "ResMgr.h"

#include "GameObject.h"
#include "Transform.h"
#include "Device.h"
#include "ConstantBuffer.h"

#include "Script.h"

UINT CCollider2D::g_iColID = 0;

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_vOffsetPos(Vec3(0.f, 0.f, 0.f))
	, m_vOffsetScale(Vec3(1.f, 1.f, 1.f))
	, m_pColMesh(nullptr)
	, m_pColMtrl(nullptr)
	, m_iColID(g_iColID++)
	, m_iCollisionCount(0)
	, m_eType(COLLIDER2D_TYPE::RECT)
{
	m_pColMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider2DMtrl_0");
	SetCollider2DType(m_eType);
}

CCollider2D::CCollider2D(const CCollider2D & _other)
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_vOffsetPos(_other.m_vOffsetPos)
	, m_vOffsetScale(_other.m_vOffsetScale)
	, m_eType(_other.m_eType)
	, m_iCollisionCount(0)
	, m_pColMesh(_other.m_pColMesh)
	, m_pColMtrl(_other.m_pColMtrl)
	, m_matColWorld(_other.m_matColWorld)
	, m_iColID(g_iColID++)				
{
}

void CCollider2D::operator=(const CCollider2D & _other)
{
	UINT iColID = m_iColID;
	memcpy(this, &_other, sizeof(CCollider2D));
	m_iColID = iColID;
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::update()
{
}

void CCollider2D::finalupdate()
{
	if (!IsActive())
		return;

	Vec3 vFinalPos = m_vOffsetPos;
	vFinalPos = vFinalPos / Transform()->GetWorldScale(); // GetWorldScale() ·Î º¯°æ

	Matrix matTranslation = XMMatrixTranslation(vFinalPos.x, vFinalPos.y, vFinalPos.z);
	Matrix matScale = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);

	m_matColWorld = matScale * matTranslation;
	m_matColWorld *= Transform()->GetWorldMat();
}

void CCollider2D::render()
{
	if (!IsActive())
		return;

	static CConstantBuffer* pCB = CDevice::GetInst()->GetCB(CONST_REGISTER::b0);

	g_transform.matWorld = m_matColWorld;	
	CDevice::GetInst()->SetConstBufferToRegister(pCB, pCB->AddData(&g_transform));	
	
	m_pColMtrl->UpdateData();
	m_pColMesh->render();

	memset(&m_matColWorld, 0, sizeof(Matrix));
}

void CCollider2D::SetCollider2DType(COLLIDER2D_TYPE _eType)
{
	m_eType = _eType;

	if (COLLIDER2D_TYPE::RECT == m_eType)
	{
		m_pColMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ColRectMesh");
	}
	else if (COLLIDER2D_TYPE::CIRCLE == m_eType)
	{
		m_pColMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ColCircleMesh");
	}
}

void CCollider2D::OnCollisionEnter(CCollider2D * _pOther)
{	
	m_iCollisionCount += 1;

	const vector<CScript*>& vecScripts = GetObj()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->OnCollisionEnter(_pOther);
	}
}

void CCollider2D::OnCollision(CCollider2D * _pOther)
{
	if (0 < m_iCollisionCount)
	{
		m_pColMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider2DMtrl_1");
	}

	const vector<CScript*>& vecScripts = GetObj()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->OnCollision(_pOther);
	}	
}

void CCollider2D::OnCollisionExit(CCollider2D * _pOther)
{	
	m_iCollisionCount -= 1;
	if(m_iCollisionCount == 0)
		m_pColMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Collider2DMtrl_0");

	const vector<CScript*>& vecScripts = GetObj()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->OnCollisionExit(_pOther);
	}
}


void CCollider2D::SaveToScene(FILE * _pFile)
{
	UINT iType = (UINT)GetComponentType();
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_eType, sizeof(UINT), 1, _pFile);
}

void CCollider2D::LoadFromScene(FILE * _pFile)
{
	fread(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fread(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
	fread(&m_eType, sizeof(UINT), 1, _pFile);
	SetCollider2DType(m_eType);
}