#include "stdafx.h"
#include "IParticleSystem.h"

#include "StructuredBuffer.h"
#include "ResMgr.h"
#include "KeyMgr.h"
#include "TimeMgr.h"

#include "Transform.h"

CIParticleSystem::CIParticleSystem()
	: CComponent(COMPONENT_TYPE::IPARTICLESYSTEM)
	, m_pParticleBuffer(nullptr)
	, m_pSharedBuffer(nullptr)
	, m_iMaxParticle(1000)
	, m_fFrequency(0.002f)
	, m_fAccTime(0.f)
	, m_fMinLifeTime(0.5f)
	, m_fMaxLifeTime(1.f)
	, m_fMinSpeed(100)
	, m_fMaxSpeed(50.f)
	, m_fStartScale(20.f)
	, m_fEndScale(10.f)
	, m_vStartColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_vEndColor(Vec4(1.0f, 1.0f, 1.0f, 0.2f))
{
	// 구조화 버퍼 생성
	m_pParticleBuffer = new CStructuredBuffer;
	m_pParticleBuffer->Create(sizeof(tParticle), m_iMaxParticle, nullptr);

	m_pSharedBuffer = new CStructuredBuffer;
	m_pSharedBuffer->Create(sizeof(tParticleShared), 1, nullptr);

	// 사각형 Mesh
	m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh");

	// Material
	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleMtrl");
	Ptr<CTexture> pParticle = CResMgr::GetInst()->Load<CTexture>(L"Texture\\Particle\\Ice.png", L"Texture\\Particle\\Ice.png");
	m_pMtrl->SetData(SHADER_PARAM::TEX_0, pParticle.GetPointer());

	// ParticleUpdate
	m_pUpdateMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleUpdateMtrl");
}

CIParticleSystem::~CIParticleSystem()
{
	SAFE_DELETE(m_pParticleBuffer);
	SAFE_DELETE(m_pSharedBuffer);
}

void CIParticleSystem::finalupdate()
{
	// 추가될 파티클 개수 계산
	m_fAccTime += DT;

	int iAdd = 0;
	if (m_fFrequency < m_fAccTime)
	{
		m_fAccTime = m_fAccTime - m_fFrequency;
		iAdd = 1;
	}

	m_pParticleBuffer->UpdateRWData(UAV_REGISTER::u0);
	m_pSharedBuffer->UpdateRWData(UAV_REGISTER::u1);

	m_pUpdateMtrl->SetData(SHADER_PARAM::INT_0, &m_iMaxParticle);
	m_pUpdateMtrl->SetData(SHADER_PARAM::INT_1, &iAdd);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_0, &m_fMinLifeTime);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_1, &m_fMaxLifeTime);

	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_2, &m_fMinSpeed);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_3, &m_fMaxSpeed);

	m_pUpdateMtrl->Dispatch(1, 1, 1);
}

void CIParticleSystem::render()
{
	Transform()->UpdateData();

	m_pParticleBuffer->UpdateData(TEXTURE_REGISTER::t10);

	m_pMtrl->SetData(SHADER_PARAM::VEC4_0, &m_vStartColor);
	m_pMtrl->SetData(SHADER_PARAM::VEC4_1, &m_vEndColor);
	m_pMtrl->SetData(SHADER_PARAM::FLOAT_0, &m_fStartScale);
	m_pMtrl->SetData(SHADER_PARAM::FLOAT_1, &m_fEndScale);

	m_pMtrl->UpdateData();
	m_pMesh->render_particle(m_iMaxParticle);
}

void CIParticleSystem::SaveToScene(FILE* _pFile)
{
}

void CIParticleSystem::LoadFromScene(FILE* _pFile)
{
}
