#include "stdafx.h"
#include "PlayerScript.h"

#include "BulletScript.h"
#include "TestScript.h"
#include "Scene.h"

CPlayerScript::CPlayerScript(CGameObject* Object, bool player)
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pOriginMtrl(nullptr)
	, m_pCloneMtrl(nullptr)
{
	pObject = Object;
	status = new PlayerStatus();
	isPlayer = player;
}

CPlayerScript::~CPlayerScript()
{
	delete status;
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
	else
		runState = PlayerState::P_RRun;

	return runState;
}

void CPlayerScript::getDamage(float damage)
<<<<<<< HEAD
{
	if (!isPlayer)
	{
		int a = 3;
	}
	// ≈◊Ω∫∆Æ
	status->hp -= damage;
}

void CPlayerScript::update()
{
	if (!status->IsDead)
	{
		// «√∑π¿ÃæÓ ¿œΩ√
		if (isPlayer)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vRot = Transform()->GetLocalRot();
			POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
			bool isMove = false;
			int keyHold[4] = { 0, };

			//////////////////////////////////////////////////////////
			// ∏∂øÏΩ∫ πÊ«‚¿∏∑Œ «√∑π¿ÃæÓ πÊ«‚ º≥¡§
			// √—æÀ πÊ«‚ ∞ËªÍ
			Vec3 vMousePos;
			vMousePos.x = (((2.0f * ptMousePos.x) / FRAME_BUFFER_WIDTH) - 1) / g_transform.matProj._11;
			vMousePos.y = -(((2.0f * ptMousePos.y) / FRAME_BUFFER_HEIGHT) - 1) / g_transform.matProj._22;
			vMousePos.z = 1.0f;

			//XMMATRIX m;
			//XMVECTOR xmVec = XMMatrixDeterminant(m);

			float a1 = g_transform.matViewInv._41;
			float a2 = g_transform.matViewInv._42;
			float a3 = g_transform.matViewInv._43;
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

			// «√∑π¿ÃæÓ ¿Ãµø π◊ πÊ«‚ º≥¡§
			Vec3 playerDir;
			playerDir.x = vBulletTargetPos.x - vPos.x;
			playerDir.y = 0;
			playerDir.z = vBulletTargetPos.z - vPos.z;
			playerDir = playerDir.Normalize();

			if (!status->IsRoll)
			{
				if (KEY_HOLD(KEY_TYPE::KEY_W))
				{
					keyHold[0] = 1;
					vPos.z += DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(0.f, 0.f, 1.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(0.f, 0.f, 1.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_S))
				{
					keyHold[1] = 1;
					vPos.z -= DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(0.f, 0.f, -1.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(0.f, 0.f, -1.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_A))
				{
					keyHold[2] = 1;
					vPos.x -= DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_D))
				{
					keyHold[3] = 1;
					vPos.x += DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, 0.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, 0.f));
				}
			}
			// ±∏∏£±‚ ªÛ≈¬¿œ∂ß ¿Ãµø
			// Ω¨«¡∆Æ ¥©∏¶ ∂ß ∏∂øÏΩ∫ πÊ«‚¿∏∑Œ ±∏∏£±‚ ≥°≥Ø∂ß ±Ó¡ˆ ∞Ì¡§ ¿Ãµø
			else
			{
				vPos += DT * rollDir * status->speed;
			}
=======
{
<<<<<<< HEAD
	if (!isPlayer)
	{
		int a = 3;
	}
	// ≈◊Ω∫∆Æ
	status->hp -= damage;
}

void CPlayerScript::update()
{
	if (!status->IsDead)
	{
		// «√∑π¿ÃæÓ ¿œΩ√
		if (isPlayer)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vRot = Transform()->GetLocalRot();
			POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
			bool isMove = false;
			int keyHold[4] = { 0, };

			//////////////////////////////////////////////////////////
			// ∏∂øÏΩ∫ πÊ«‚¿∏∑Œ «√∑π¿ÃæÓ πÊ«‚ º≥¡§
			// √—æÀ πÊ«‚ ∞ËªÍ
			Vec3 vMousePos;
			vMousePos.x = (((2.0f * ptMousePos.x) / FRAME_BUFFER_WIDTH) - 1) / g_transform.matProj._11;
			vMousePos.y = -(((2.0f * ptMousePos.y) / FRAME_BUFFER_HEIGHT) - 1) / g_transform.matProj._22;
			vMousePos.z = 1.0f;

			//XMMATRIX m;
			//XMVECTOR xmVec = XMMatrixDeterminant(m);

			float a1 = g_transform.matViewInv._41;
			float a2 = g_transform.matViewInv._42;
			float a3 = g_transform.matViewInv._43;
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

			// «√∑π¿ÃæÓ ¿Ãµø π◊ πÊ«‚ º≥¡§
			Vec3 playerDir;
			playerDir.x = vBulletTargetPos.x - vPos.x;
			playerDir.y = 0;
			playerDir.z = vBulletTargetPos.z - vPos.z;
			playerDir = playerDir.Normalize();

			if (!status->IsRoll)
			{
				if (KEY_HOLD(KEY_TYPE::KEY_W))
				{
					keyHold[0] = 1;
					vPos.z += DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(0.f, 0.f, 1.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(0.f, 0.f, 1.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_S))
				{
					keyHold[1] = 1;
					vPos.z -= DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(0.f, 0.f, -1.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(0.f, 0.f, -1.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_A))
				{
					keyHold[2] = 1;
					vPos.x -= DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f));
				}

				if (KEY_HOLD(KEY_TYPE::KEY_D))
				{
					keyHold[3] = 1;
					vPos.x += DT * status->speed;
					isMove = true;
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, 0.f)) && !status->IsRoll)
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, 0.f));
				}
			}
			// ±∏∏£±‚ ªÛ≈¬¿œ∂ß ¿Ãµø
			// Ω¨«¡∆Æ ¥©∏¶ ∂ß ∏∂øÏΩ∫ πÊ«‚¿∏∑Œ ±∏∏£±‚ ≥°≥Ø∂ß ±Ó¡ˆ ∞Ì¡§ ¿Ãµø
			else
			{
				vPos += DT * rollDir * status->speed;
			}
=======
	Vec3 vPos = CNetworkMgr::GetInst()->playerPos;
	status.state = CNetworkMgr::GetInst()->m_ePState;
	PlayerState playerState = CNetworkMgr::GetInst()->m_ePState;
	Vec3 vRot = Transform()->GetLocalRot();
	POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
	bool isMove = false;
	int keyHold[4] = { 0, };

	//////////////////////////////////////////////////////////
	// ∏∂øÏΩ∫ πÊ«‚¿∏∑Œ «√∑π¿ÃæÓ πÊ«‚ º≥¡§
	// √—æÀ πÊ«‚ ∞ËªÍ
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

	// «√∑π¿ÃæÓ ¿Ãµø π◊ πÊ«‚ º≥¡§
	Vec3 playerDir;
	playerDir.x = vBulletTargetPos.x - vPos.x;
	playerDir.y = 0;
	playerDir.z = vBulletTargetPos.z - vPos.z;
	playerDir = playerDir.Normalize();

	if (KEY_HOLD(KEY_TYPE::KEY_W))
	{
		keyHold[0] = 1;

		CNetworkMgr::GetInst()->send_Key_packet(EKEY_EVENT::DOWN_UP, vRot);
		//vPos.z += DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(0.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(0.f, 0.f, 1.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_S))
	{
		keyHold[1] = 1;

		CNetworkMgr::GetInst()->send_Key_packet(EKEY_EVENT::DOWN_DOWN, vRot);
		//vPos.z -= DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(0.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(0.f, 0.f, -1.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_A))
	{
		keyHold[2] = 1;
		CNetworkMgr::GetInst()->send_Key_packet(EKEY_EVENT::DOWN_LEFT, vRot);
		//vPos.x -= DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, 0.f));
	}

	if (KEY_HOLD(KEY_TYPE::KEY_D))
	{
		keyHold[3] = 1;
		CNetworkMgr::GetInst()->send_Key_packet(EKEY_EVENT::DOWN_RIGHT, vRot);
		//vPos.x += DT * status.speed;
		isMove = true;
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, 0.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, 0.f));
	}





	// ¥Î∞¢º± æ÷¥œ∏ﬁ¿Ãº« √º≈©
	//¡¬ªÛ
	if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 1 && keyHold[3] == 0)
	{
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f));
	}
	//øÏªÛ
	else if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 0 && keyHold[3] == 1)
	{
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, 1.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, 1.f));
	}
	//¡¬«œ
	else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 1 && keyHold[3] == 0)
	{
		if (status.state != setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f));
	}
	//øÏ«œ
	else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 0 && keyHold[3] == 1)
	{
		if (status.state != setRunAni(playerDir, Vec3(1.f, 0.f, -1.f)))
			status.state = setRunAni(playerDir, Vec3(1.f, 0.f, -1.f));
	}


	// æ∆¿ÃµÈ æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬¿Œ¡ˆ »Æ¿Œ
	if (!isMove)
	{
		if (status.state != PlayerState::P_Idle)
			status.state = PlayerState::P_Idle;
	}

	if (CNetworkMgr::GetInst()->m_isChange) {


		CNetworkMgr::GetInst()->send_Key_packet(EKEY_EVENT::NO_EVENT, vRot);

	}
	// æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬∞° πŸ≤Óæ˙¥¬¡ˆ »Æ¿Œ
	if (previousState != status.state)
	{
		isAniChange = true;
		previousState = status.state;
	}
	else
		isAniChange = false;
>>>>>>> parent of 0651e18 (ÌÅ¥ÎùºÏù¥Ïñ∏Ìä∏ Î∞∞Ïó¥Ï∂îÍ∞ÄÌñàÎçîÎãà Ïò§Î•òÍ∞ÄÎÇ®)
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e

			// ±∏∏£±‚ 
			if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
			{
				if (!status->IsRoll)
				{
					status->IsRoll = true;
					status->state = PlayerState::P_Roll;

					// ±∏∏£±‚ ≈∞ ¿‘∑¬ ¥ÁΩ√ πÊ«‚¿ª ¿˙¿Â«ÿ π´¡∂∞« ±∏∏£±‚∞° ≥°≥Ø∂ß ±Ó¡ˆ ±◊ πÊ«‚¿∏∑Œ ¿Ãµø µ«∞‘∏∏ «“∞≈¿”
					rollDir = playerDir;
				}
			}
<<<<<<< HEAD

			// ¥Î∞¢º± æ÷¥œ∏ﬁ¿Ãº« √º≈©
			if (!status->IsRoll)
			{
				//¡¬ªÛ
				if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f));
				}
				//øÏªÛ
				else if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, 1.f));
				}
				//¡¬«œ
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f));
				}
				//øÏ«œ
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, -1.f));
				}
			}

			// æ∆¿ÃµÈ æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬¿Œ¡ˆ »Æ¿Œ
			if (!isMove && !status->IsRoll)
			{
				if (status->state != PlayerState::P_Idle)
					status->state = PlayerState::P_Idle;
			}

			// æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬∞° πŸ≤Óæ˙¥¬¡ˆ »Æ¿Œ
			if (previousState != status->state)
			{
				isAniChange = true;
				previousState = status->state;
			}
			else
				isAniChange = false;


=======

			// ¥Î∞¢º± æ÷¥œ∏ﬁ¿Ãº« √º≈©
			if (!status->IsRoll)
			{
				//¡¬ªÛ
				if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f));
				}
				//øÏªÛ
				else if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, 1.f));
				}
				//¡¬«œ
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f));
				}
				//øÏ«œ
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, -1.f));
				}
			}

			// æ∆¿ÃµÈ æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬¿Œ¡ˆ »Æ¿Œ
			if (!isMove && !status->IsRoll)
			{
				if (status->state != PlayerState::P_Idle)
					status->state = PlayerState::P_Idle;
			}

			// æ÷¥œ∏ﬁ¿Ãº« ªÛ≈¬∞° πŸ≤Óæ˙¥¬¡ˆ »Æ¿Œ
			if (previousState != status->state)
			{
				isAniChange = true;
				previousState = status->state;
			}
			else
				isAniChange = false;


<<<<<<< HEAD
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e
			// «√∑π¿ÃæÓ ¿ßƒ° πÊ«‚ º≥¡§
			if (vPos.x > 4990)
				vPos.x = 4990;
			if (vPos.x < -4990)
				vPos.x = -4990;
			if (vPos.z > 4990)
				vPos.z = 4990;
			if (vPos.z < -4990)
				vPos.z = -4990;
			Transform()->SetLocalPos(vPos);
			Transform()->SetLocalRot(vRot);
<<<<<<< HEAD


			//±∏∏£±‚ æ÷¥œ∏ﬁ¿Ãº« º≥¡§
			if (isAniChange)
=======


			//±∏∏£±‚ æ÷¥œ∏ﬁ¿Ãº« º≥¡§
			if (isAniChange)
=======
	//æ÷¥œ∏ﬁ¿Ãº« º≥¡§
	if (isAniChange)
	{
		if (isMove)
		{
			float revise = 0;
			// πÊ«‚∫∞ ¥ﬁ∏Æ±‚ æ÷¥œ∏ﬁ¿Ãº« º≥¡§
			Ptr<CMeshData> pMeshData;
			if (status.state == PlayerState::P_FRun) {
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRun.mdat", L"MeshData\\SoldierRun.mdat");

			}
			else if (status.state == PlayerState::P_BRun)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierBRun.mdat", L"MeshData\\SoldierBRun.mdat");

			else if (status.state == PlayerState::P_LRun)
>>>>>>> parent of 0651e18 (ÌÅ¥ÎùºÏù¥Ïñ∏Ìä∏ Î∞∞Ïó¥Ï∂îÍ∞ÄÌñàÎçîÎãà Ïò§Î•òÍ∞ÄÎÇ®)
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e
			{
				if (status->IsRoll)
				{
					Ptr<CMeshData> pMeshData;

					status->state = PlayerState::P_Roll;
					pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRoll.mdat", L"MeshData\\SoldierRoll.mdat");

					if (pMeshData != NULL)
					{
						pObject->ChangeAnimation(pMeshData);
					}

					// ¿Ã∞« ∏µ® ««∫ø ¿ﬂ∏¯º≥¡§«ÿº≠ ¿”Ω√∑Œ º≥¡§
					// ºˆ¡§µ«∏È ¡ˆøÔ ∞Õ
					Vec3 temp = pObject->Transform()->GetLocalPos();
					pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
				}

				//æ÷¥œ∏ﬁ¿Ãº« º≥¡§
				else
				{
					if (isMove)
					{
						float revise = 0;
						Ptr<CMeshData> pMeshData;

						// πÊ«‚∫∞ ¥ﬁ∏Æ±‚ æ÷¥œ∏ﬁ¿Ãº« º≥¡§
						if (status->state == PlayerState::P_FRun)
							pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRun.mdat", L"MeshData\\SoldierRun.mdat");

						else if (status->state == PlayerState::P_BRun)
							pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierBRun.mdat", L"MeshData\\SoldierBRun.mdat");

						else if (status->state == PlayerState::P_LRun)
						{
							pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierLRun.mdat", L"MeshData\\SoldierLRun.mdat");
							revise = 4;
						}

						else if (status->state == PlayerState::P_RRun)
						{
							pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierRRun.mdat", L"MeshData\\SoldierRRun.mdat");
							revise = 4;
						}

						if (pMeshData != NULL)
						{
							pObject->ChangeAnimation(pMeshData);
						}

						// ¿Ã∞« ∏µ® ««∫ø ¿ﬂ∏¯º≥¡§«ÿº≠ ¿”Ω√∑Œ º≥¡§
						// ºˆ¡§µ«∏È ¡ˆøÔ ∞Õ
						Vec3 temp = pObject->Transform()->GetLocalPos();
						pObject->Transform()->SetLocalPos(Vec3(temp.x, 53.f + revise, temp.z));
					}

					else
					{
						Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
						pObject->ChangeAnimation(pMeshData);

						// ¿Ã∞« ∏µ® ««∫ø ¿ﬂ∏¯º≥¡§«ÿº≠ ¿”Ω√∑Œ º≥¡§
						// ºˆ¡§µ«∏È ¡ˆøÔ ∞Õ
						Vec3 temp = pObject->Transform()->GetLocalPos();
						pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
					}
				}
			}

<<<<<<< HEAD
			// ±∏∏£±‚ ƒ ≈∏¿” √º≈©
			if (status->IsRoll)
=======
<<<<<<< HEAD
			// ±∏∏£±‚ ƒ ≈∏¿” √º≈©
			if (status->IsRoll)
=======
			else if (status.state == PlayerState::P_RRun)
>>>>>>> parent of 0651e18 (ÌÅ¥ÎùºÏù¥Ïñ∏Ìä∏ Î∞∞Ïó¥Ï∂îÍ∞ÄÌñàÎçîÎãà Ïò§Î•òÍ∞ÄÎÇ®)
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e
			{
				status->RollCoolTime += DT;
				if (status->RollCoolTime >= shiftCoolTime)
				{
					status->IsRoll = false;
					status->RollCoolTime = 0;
					status->state = PlayerState::p_None;

					/*	for (int i = 0; i < 4; i++)
							rollDir[i] = 0;*/

					vPos += 20 * rollDir;
					pObject->Transform()->SetLocalPos(vPos);
				}
			}


			// ¿Ã∞« ∏µ® ««∫ø ¿ﬂ∏¯º≥¡§«ÿº≠ ¿”Ω√∑Œ º≥¡§
			// ºˆ¡§µ«∏È ¡ˆøÔ ∞Õ
			if (status->state == PlayerState::p_None)
			{
				Vec3 temp = pObject->Transform()->GetLocalPos();
				pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
			}


<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e
			// √—æÀ πÊ«‚ & «√∑π¿ÃæÓ πÊ«‚
			float temp = atan2(vBulletTargetPos.z - vPos.z, vBulletTargetPos.x - vPos.x);
			if (!status->IsRoll)
				Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));
<<<<<<< HEAD
=======
=======
			//// ¿Ã∞« ∏µ® ««∫ø ¿ﬂ∏¯º≥¡§«ÿº≠ ¿”Ω√∑Œ º≥¡§
			//// ºˆ¡§µ«∏È ¡ˆøÔ ∞Õ
			//Vec3 temp = pObject->Transform()->GetLocalPos();
			//pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
		}
	}
	if (status.state != PlayerState::P_Idle){
		vPos.y = 53.f;
		Transform()->SetLocalPos(vPos);
	}
	else {
		Transform()->SetLocalPos(vPos);
	}
>>>>>>> parent of 0651e18 (ÌÅ¥ÎùºÏù¥Ïñ∏Ìä∏ Î∞∞Ïó¥Ï∂îÍ∞ÄÌñàÎçîÎãà Ïò§Î•òÍ∞ÄÎÇ®)
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e


			// √—æÀ
			//


			if (KEY_TAB(KEY_TYPE::KEY_LBTN) && !status->IsRoll)
			{
				bulletHeight = 100;
<<<<<<< HEAD

				vBulletTargetPos.x = (bulletHeight - vPickRayOrig.y) * vPickRayDir.x / vPickRayDir.y + vPickRayOrig.x;
				vBulletTargetPos.z = (bulletHeight - vPickRayOrig.y) * vPickRayDir.z / vPickRayDir.y + vPickRayOrig.z;

				Vec3 vBulletDir;
				vBulletDir.x = vBulletTargetPos.x - vPos.x;
				vBulletDir.y = 0;
				vBulletDir.z = vBulletTargetPos.z - vPos.z;

				//printf("%f	%f	%f\n", vBulletDir.x, vBulletDir.y, vBulletDir.z);

				Vec3 vNBulletDir = vBulletDir.Normalize();

				// √—æÀ ΩÓ±‚
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
				//pBullet->Collider2D()->SetOffsetPos(Vec3(0.f, -bulletHeight - 5000.f, 0.f));

				pBullet->AddComponent(new CBulletScript(vNBulletDir, status->bulletState));

				CreateObject(pBullet, L"Bullet");
			}
		}
=======

				vBulletTargetPos.x = (bulletHeight - vPickRayOrig.y) * vPickRayDir.x / vPickRayDir.y + vPickRayOrig.x;
				vBulletTargetPos.z = (bulletHeight - vPickRayOrig.y) * vPickRayDir.z / vPickRayDir.y + vPickRayOrig.z;

				Vec3 vBulletDir;
				vBulletDir.x = vBulletTargetPos.x - vPos.x;
				vBulletDir.y = 0;
				vBulletDir.z = vBulletTargetPos.z - vPos.z;

				//printf("%f	%f	%f\n", vBulletDir.x, vBulletDir.y, vBulletDir.z);

				Vec3 vNBulletDir = vBulletDir.Normalize();

				// √—æÀ ΩÓ±‚
				CGameObject* pBullet = new CGameObject;
				pBullet->SetName(L"Bullet Object");

				pBullet->AddComponent(new CTransform());
				pBullet->Transform()->SetLocalPos(Vec3(vPos.x, bulletHeight, vPos.z));
				pBullet->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

				pBullet->AddComponent(new CMeshRender);
				pBullet->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
				pBullet->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"));

<<<<<<< HEAD
				pBullet->AddComponent(new CCollider2D);
				pBullet->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
				//pBullet->Collider2D()->SetOffsetPos(Vec3(0.f, -bulletHeight - 5000.f, 0.f));
=======
		pBullet->AddComponent(new CBulletScript(vNBulletDir, status.bulletState));
>>>>>>> parent of 0651e18 (ÌÅ¥ÎùºÏù¥Ïñ∏Ìä∏ Î∞∞Ïó¥Ï∂îÍ∞ÄÌñàÎçîÎãà Ïò§Î•òÍ∞ÄÎÇ®)

				pBullet->AddComponent(new CBulletScript(vNBulletDir, status->bulletState));

				CreateObject(pBullet, L"Bullet");
			}
		}

		// «√∑π¿ÃæÓ ¡◊¿Ω √º≈©
		if (status->hp <= 0)
		{
			status->IsDead = true;

			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierDying.mdat", L"MeshData\\SoldierDying.mdat");
			pObject->ChangeAnimation(pMeshData);
		}
	}
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e

		// «√∑π¿ÃæÓ ¡◊¿Ω √º≈©
		if (status->hp <= 0)
		{
			status->IsDead = true;

<<<<<<< HEAD
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierDying.mdat", L"MeshData\\SoldierDying.mdat");
			pObject->ChangeAnimation(pMeshData);
		}
	}


=======
>>>>>>> 6eec2dd8a504b0747f527b951430a77b4910601e
	// ªÁ∂Û¡ˆ¥¬ Ω√∞£ √º≈©
	if (status->IsDead)
	{
		status->disappearCnt += DT;
		{
			if (status->disappearCnt >= 3.4f)
			{
				status->isDisappear = true;
				Vec3 vPos = pObject->Transform()->GetLocalPos();
				pObject->Transform()->SetLocalPos(Vec3(20000.f, 20000.f, 20000.f));
			}
		}
	}
}
