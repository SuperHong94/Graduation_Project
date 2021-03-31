#include "stdafx.h"
#include "PlayerScript.h"

#include "BulletScript.h"
#include "TestScript.h"
#include "Scene.h"

CPlayerScript::CPlayerScript(CGameObject* Object)
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pOriginMtrl(nullptr)
	, m_pCloneMtrl(nullptr)
{
	pObject = Object;
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


PlayerState CPlayerScript::setRunAni(Vec3 dir, Vec3 axis)
{
	PlayerState runState;

	Vec3 axis1 = axis.Normalize();
	Vec2 dir2, axis2;
	dir2.x = dir.x;
	dir2.y = dir.z;
	axis2.x = axis1.x;
	axis2.y = axis1.z;

	Vec2 dot = XMVector2Dot(dir2, axis2);

	Vec3 cross = XMVector3Cross(dir, axis1);
	if (dot.x >= 0.8)
		runState = PlayerState::P_FRun;
	else if (dot.x <= -0.8)
		runState = PlayerState::P_BRun;
	else if (cross.y < 0)
		runState = PlayerState::P_LRun;
	else if (cross.y > 0)
		runState = PlayerState::P_RRun;

	return runState;
}

void CPlayerScript::update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();
	POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
	bool isMove = false;
	int keyHold[4] = { 0, };

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

	// 플레이어 이동 및 방향 설정
	Vec3 playerDir;
	playerDir.x = vBulletTargetPos.x - vPos.x;
	playerDir.y = 0;
	playerDir.z = vBulletTargetPos.z - vPos.z;
	playerDir = playerDir.Normalize();

	if (KEY_HOLD(KEY_TYPE::KEY_W))
	{
		keyHold[0] = 1;
		vPos.z += DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(0.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(0.f, 0.f, 1.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_S))
	{
		keyHold[1] = 1;
		vPos.z -= DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(0.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(0.f, 0.f, -1.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_A))
	{
		keyHold[2] = 1;
		vPos.x -= DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_D))
	{
		keyHold[3] = 1;
		vPos.x += DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, 0.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, 0.f));
	}

	// 대각선 애니메이션 체크
	//좌상
	if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 1 && keyHold[3] == 0)
	{
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f));
	}
	//우상
	else if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 0 && keyHold[3] == 1)
	{
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, 1.f));
	}
	//좌하
	else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 1 && keyHold[3] == 0)
	{
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f));
	}
	//우하
	else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 0 && keyHold[3] == 1)
	{
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, -1.f));
	}


	// 아이들 애니메이션 상태인지 확인
	if (!isMove)
	{
		if (status.state != PlayerState::P_Idle)
			status.state = PlayerState::P_Idle;
	}


	// 애니메이션 상태가 바뀌었는지 확인
	if (previousState != status.state)
	{
		isAniChange = true;
		previousState = status.state;
	}
	else
		isAniChange = false;

	// 플레이어 위치 방향 설정
	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);

	//애니메이션 설정
	if (isAniChange)
	{
		if (isMove)
		{
			// 방향별 달리기 애니메이션 설정
			Ptr<CMeshData> pMeshData;
			if (status.state == PlayerState::P_FRun)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRun.mdat", L"MeshData\\SoldierRun.mdat");

			else if (status.state == PlayerState::P_BRun)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierBRun.mdat", L"MeshData\\SoldierBRun.mdat");

			else if (status.state == PlayerState::P_LRun)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierLRun.mdat", L"MeshData\\SoldierLRun.mdat");

			else if (status.state == PlayerState::P_RRun)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRRun.mdat", L"MeshData\\SoldierRRun.mdat");

			if (pMeshData != NULL)
			{
				pObject->ChangeAnimation(pMeshData);
			}

			// 이건 모델 피봇 잘못설정해서 임시로 설정
			// 수정되면 지울 것
			Vec3 temp = pObject->Transform()->GetLocalPos();
			pObject->Transform()->SetLocalPos(Vec3(temp.x, 53.f, temp.z));
		}

		else
		{
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
			pObject->ChangeAnimation(pMeshData);

			// 이건 모델 피봇 잘못설정해서 임시로 설정
			// 수정되면 지울 것
			Vec3 temp = pObject->Transform()->GetLocalPos();
			pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
		}
	}

	float temp = atan2(vBulletTargetPos.z - vPos.z, vBulletTargetPos.x - vPos.x);
	Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));

	if (KEY_TAB(KEY_TYPE::KEY_LBTN))
	{
		bulletHeight = 100;

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

		pBullet->AddComponent(new CTransform());
		pBullet->Transform()->SetLocalPos(Vec3(vPos.x, bulletHeight, vPos.z));
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
