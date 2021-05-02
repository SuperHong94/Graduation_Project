#include "stdafx.h"
#include "PlayerScript.h"

#include "BulletScript.h"
#include "TestScript.h"
#include "Scene.h"

#include "RenderMgr.h"

CPlayerScript::CPlayerScript(CGameObject* Object, bool player)
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_pOriginMtrl(nullptr)
	, m_pCloneMtrl(nullptr)
{
	pObject = Object;
	status = new PlayerStatus();
	isPlayer = player;

	// �Ѿ� ������
	for (int i = 0; i < BulletCnt; i++)
	{
		pBullet[i] = new CGameObject;
		pBullet[i]->SetName(L"Bullet Object");

		pBullet[i]->AddComponent(new CTransform());
		pBullet[i]->Transform()->SetLocalPos(Vec3(-20000.0f, 20000.0f, 20000.0f));
		pBullet[i]->AddComponent(new CMeshRender);
		pBullet[i]->AddComponent(new CCollider2D);
		pBullet[i]->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
		//pBullet->Collider2D()->SetOffsetPos(Vec3(0.f, -bulletHeight - 5000.f, 0.f));

		pBullet[i]->AddComponent(new CBulletScript(Vec3(0,0,0), status->bulletState));

		CreateObject(pBullet[i], L"Bullet");
	}
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
{
	if (!isPlayer)
	{
		int a = 3;
	}
	// �׽�Ʈ
	status->hp -= damage;
}

void CPlayerScript::update()
{
	if (!status->IsDead)
	{
		// �÷��̾� �Ͻ�
		if (isPlayer)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vRot = Transform()->GetLocalRot();
			POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();
			bool isMove = false;
			int keyHold[4] = { 0, };

			//////////////////////////////////////////////////////////
			// ���콺 �������� �÷��̾� ���� ����
			// �Ѿ� ���� ���
			Vec3 vMousePos;
			tResolution res = CRenderMgr::GetInst()->GetResolution();

			vMousePos.x = (((2.0f * ptMousePos.x) / res.fWidth) - 1) / g_transform.matProj._11;
			vMousePos.y = -(((2.0f * ptMousePos.y) / res.fHeight) - 1) / g_transform.matProj._22;
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

			// �÷��̾� �̵� �� ���� ����
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
			// ������ �����϶� �̵�
			// ����Ʈ ���� �� ���콺 �������� ������ ������ ���� ���� �̵�
			else
			{
				vPos += DT * rollDir * status->speed;
			}

			// ������ 
			if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
			{
				if (!status->IsRoll)
				{
					status->IsRoll = true;
					status->state = PlayerState::P_Roll;

					// ������ Ű �Է� ��� ������ ������ ������ �����Ⱑ ������ ���� �� �������� �̵� �ǰԸ� �Ұ���
					rollDir = playerDir;
				}
			}

			// �밢�� �ִϸ��̼� üũ
			if (!status->IsRoll)
			{
				//�»�
				if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, 1.f));
				}
				//���
				else if (keyHold[0] == 1 && keyHold[1] == 0 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, 1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, 1.f));
				}
				//����
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 1 && keyHold[3] == 0)
				{
					if (status->state != setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(-1.f, 0.f, -1.f));
				}
				//����
				else if (keyHold[0] == 0 && keyHold[1] == 1 && keyHold[2] == 0 && keyHold[3] == 1)
				{
					if (status->state != setRunAni(playerDir, Vec3(1.f, 0.f, -1.f)))
						status->state = setRunAni(playerDir, Vec3(1.f, 0.f, -1.f));
				}
			}

			// ���̵� �ִϸ��̼� �������� Ȯ��
			if (!isMove && !status->IsRoll)
			{
				if (status->state != PlayerState::P_Idle)
					status->state = PlayerState::P_Idle;
			}

			// �ִϸ��̼� ���°� �ٲ������ Ȯ��
			if (previousState != status->state)
			{
				isAniChange = true;
				previousState = status->state;
			}
			else
				isAniChange = false;


			// �÷��̾� ��ġ ���� ����
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


			//������ �ִϸ��̼� ����
			if (isAniChange)
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

					// �̰� �� �Ǻ� �߸������ؼ� �ӽ÷� ����
					// �����Ǹ� ���� ��
					Vec3 temp = pObject->Transform()->GetLocalPos();
					pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
				}

				//�ִϸ��̼� ����
				else
				{
					if (isMove)
					{
						float revise = 0;
						Ptr<CMeshData> pMeshData;

						// ���⺰ �޸��� �ִϸ��̼� ����
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

						// �̰� �� �Ǻ� �߸������ؼ� �ӽ÷� ����
						// �����Ǹ� ���� ��
						Vec3 temp = pObject->Transform()->GetLocalPos();
						pObject->Transform()->SetLocalPos(Vec3(temp.x, 53.f + revise, temp.z));
					}

					else
					{
						Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
						pObject->ChangeAnimation(pMeshData);

						// �̰� �� �Ǻ� �߸������ؼ� �ӽ÷� ����
						// �����Ǹ� ���� ��
						Vec3 temp = pObject->Transform()->GetLocalPos();
						pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
					}
				}
			}

			// ������ �� Ÿ�� üũ
			if (status->IsRoll)
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


			// �̰� �� �Ǻ� �߸������ؼ� �ӽ÷� ����
			// �����Ǹ� ���� ��
			if (status->state == PlayerState::p_None)
			{
				Vec3 temp = pObject->Transform()->GetLocalPos();
				pObject->Transform()->SetLocalPos(Vec3(temp.x, 0.f, temp.z));
			}


			// �Ѿ� ���� & �÷��̾� ����
			float temp = atan2(vBulletTargetPos.z - vPos.z, vBulletTargetPos.x - vPos.x);
			if (!status->IsRoll)
				Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));


			// �Ѿ�
			//


			if (KEY_TAB(KEY_TYPE::KEY_LBTN) && !status->IsRoll)
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

				// �ؽ�ó
				Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>(L"NormalB", L"Texture\\Bullet\\NormalBullet.png");

				// �Ѿ� ��ġ
				for (int i = 0; i < BulletCnt; i++)
				{
					if (!pBullet[i]->GetScript<CBulletScript>()->GetActive())
					{
						pBullet[i]->GetScript<CBulletScript>()->SetActive(true);

						pBullet[i]->GetScript<CBulletScript>()->SetDir(vNBulletDir);

						pBullet[i]->Transform()->SetLocalPos(Vec3(vPos.x + vNBulletDir.x * 20, bulletHeight, vPos.z + vNBulletDir.z * 20));

						/*			pBullet->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
									pBullet->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"));*/

						pBullet[i]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh"));
						Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
						pBullet[i]->MeshRender()->SetMaterial(pMtrl->Clone());

						if (status->bulletState == BulletState::B_Normal)
						{
							pBullet[i]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pNormal.GetPointer());

							// �Ѿ� ũ�� ����
							pBullet[i]->Transform()->SetLocalScale(Vec3(80.f, 4.5f, 30.f));
						}
						else if (status->bulletState == BulletState::B_Fire)
						{
							pBullet[i]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pNormal.GetPointer());

							pBullet[i]->Transform()->SetLocalScale(Vec3(80.f, 2.f, 30.f));
						}
						else if (status->bulletState == BulletState::B_Ice)
						{
							pBullet[i]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pNormal.GetPointer());

							pBullet[i]->Transform()->SetLocalScale(Vec3(80.f, 2.f, 30.f));
						}
						else if (status->bulletState == BulletState::B_Thunder)
						{
							pBullet[i]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pNormal.GetPointer());

							pBullet[i]->Transform()->SetLocalScale(Vec3(80.f, 2.f, 30.f));
						}

						// �Ѿ� ���� ����
						float temp = atan2(vNBulletDir.z, vNBulletDir.x);
						pBullet[i]->Transform()->SetLocalRot(Vec3(XM_PI / 2, -temp, 0.f));

						break;
					}

				}
			}
		}

		// �÷��̾� ���� üũ
		if (status->hp <= 0)
		{
			status->IsDead = true;

			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierDying.mdat", L"MeshData\\SoldierDying.mdat");
			pObject->ChangeAnimation(pMeshData);
		}
	}


	// ������� �ð� üũ
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
