#include "stdafx.h"
#include "PlayerScript.h"

#include "BulletScript.h"
#include "TestScript.h"

CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pOriginMtrl(nullptr)
	, m_pCloneMtrl(nullptr)
{	
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::awake()
{
	m_pOriginMtrl = MeshRender()->GetSharedMaterial();
	m_pCloneMtrl = m_pOriginMtrl->Clone();

	int a = 1;
	m_pCloneMtrl->SetData(SHADER_PARAM::INT_0, &a);
}

void CPlayerScript::update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();
	POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();

	if (KEY_HOLD(KEY_TYPE::KEY_W))
	{
		vPos.z += DT * 300.f;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_S))
	{
		vPos.z -= DT * 300.f;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_A))
	{
		vPos.x -= DT * 300.f;
	}

	if (KEY_HOLD(KEY_TYPE::KEY_D))
	{
		vPos.x += DT * 300.f;
	}

	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);

	//////////////////////////////////////////////////////////
	// 마우스 방향으로 플레이어 방향 설정
	// 총알 방향 계산
	Vec3 vMousePos;
	vMousePos.x = (((2.0f * ptMousePos.x) / FRAME_BUFFER_WIDTH) - 1) / g_transform.matProj._11;
	vMousePos.y = -(((2.0f * ptMousePos.y) / FRAME_BUFFER_HEIGHT) - 1) / g_transform.matProj._22;
	vMousePos.z = 1.0f;

	//XMMATRIX m;
	//XMVECTOR xmVec = XMMatrixDeterminant(m);
	XMMATRIX InverseM = XMMatrixInverse(NULL, g_transform.matView);

	Vec3 vPickRayDir;
	vPickRayDir.x = vMousePos.x * InverseM._11 + vMousePos.y * InverseM._21 + vMousePos.z * InverseM._31;
	vPickRayDir.y = vMousePos.x * InverseM._12 + vMousePos.y * InverseM._22 + vMousePos.z * InverseM._32;
	vPickRayDir.z = vMousePos.x * InverseM._13 + vMousePos.y * InverseM._23 + vMousePos.z * InverseM._33;

	Vec3 vPickRayOrig;
	vPickRayOrig.x = InverseM._41;
	vPickRayOrig.y = InverseM._42;
	vPickRayOrig.z = InverseM._43;

	bulletHeight = 0;

	Vec3 vBulletTargetPos;
	vBulletTargetPos.x = (bulletHeight - vPickRayOrig.y) * vPickRayDir.x / vPickRayDir.y + vPickRayOrig.x;
	vBulletTargetPos.z = (bulletHeight - vPickRayOrig.y) * vPickRayDir.z / vPickRayDir.y + vPickRayOrig.z;
	//////////////////////////////////////////////////////////

	float temp = atan2(vBulletTargetPos.z - vPos.z, vBulletTargetPos.x - vPos.x);
	Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI/2, 0.f));

	if (KEY_TAB(KEY_TYPE::KEY_LBTN))
	{
		bulletHeight = vPos.y;

		vBulletTargetPos.x = (bulletHeight - vPickRayOrig.y) * vPickRayDir.x / vPickRayDir.y + vPickRayOrig.x;
		vBulletTargetPos.z = (bulletHeight - vPickRayOrig.y) * vPickRayDir.z / vPickRayDir.y + vPickRayOrig.z;

		Vec3 vBulletDir;
		vBulletDir.x = vBulletTargetPos.x - vPos.x;
		vBulletDir.y = 0;
		vBulletDir.z = vBulletTargetPos.z - vPos.z;

		//printf("%f	%f	%f\n", vBulletDir.x, vBulletDir.y, vBulletDir.z);

		Vec3 vNBulletDir = vBulletDir.Normalize();

		// 총알 쏘기
		CGameObject* pBullet = new CGameObject;
		pBullet->SetName(L"Bullet Object");

		vPos.y += 15;
		pBullet->AddComponent(new CTransform());
		pBullet->Transform()->SetLocalPos(vPos);
		pBullet->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

		pBullet->AddComponent(new CMeshRender);
		pBullet->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
		pBullet->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"));

		pBullet->AddComponent(new CCollider2D);
		pBullet->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);

		pBullet->AddComponent(new CBulletScript(vNBulletDir, 40));

		CreateObject(pBullet, L"Bullet");
	}
}
