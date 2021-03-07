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

	// z 키를 누르면 z 축 회전
	if (KEY_HOLD(KEY_TYPE::KEY_Z))
	{
		vRot.z += DT * XM_PI;

		// 복사 메테리얼을 MeshRender 에 세팅
		MeshRender()->SetMaterial(m_pCloneMtrl);
	}
	else if (KEY_AWAY(KEY_TYPE::KEY_Z))
	{
		// z 키를 떼면 원레 메테리얼로 돌아감
		MeshRender()->SetMaterial(m_pOriginMtrl);
	}

	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);

	if (KEY_TAB(KEY_TYPE::KEY_LBTN))
	{
		// 총알 방향 계산
		POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
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

		bulletHeight = vPos.y;

		Vec3 vBulletPos;
		vBulletPos.x = (bulletHeight - vPickRayOrig.y) * vPickRayDir.x / vPickRayDir.y + vPickRayOrig.x;
		vBulletPos.z = (bulletHeight - vPickRayOrig.y) * vPickRayDir.z / vPickRayDir.y + vPickRayOrig.z;

		Vec3 vBulletDir;
		vBulletDir.x = vBulletPos.x - vPos.x;
		vBulletDir.y = 0;
		vBulletDir.z = vBulletPos.z - vPos.z;

		printf("%f	%f	%f\n", vBulletDir.x, vBulletDir.y, vBulletDir.z);

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
