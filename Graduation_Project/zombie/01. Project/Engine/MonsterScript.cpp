#include "stdafx.h"
#include "MonsterScript.h"
#include "BulletScript.h"
#include "ItemScript.h"
#include "BossScript.h"

CMonsterScript::CMonsterScript(CGameObject* targetObject[], int ntargetNum, CGameObject* Object, CScene* pscene)
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
{
	targetNum = ntargetNum;
	for (int i = 0; i < targetNum; i++)
	{
		targetObjects[i] = new CGameObject;
		targetObjects[i] = targetObject[i];
	}

	this->SetName(L"MonsterScript");
	pObject = Object;
	status = new MonsterStatus;
	status->state = MonsterState::M_Respawn;
	status->TargetObject = targetObject[0];
	pScene = pscene;

	root = new Sequence;
	sequence1 = new Sequence;
	CCheckRange = new CheckPlayerInRange(status, pObject, pScene);
	CCheckAttackRange = new CheckPlayerInAttackRange(status, pObject, pScene);
	CAttackPlayer = new AttackPlayer(status, pObject, pScene);

	root->addChild(sequence1);

	sequence1->addChild(CCheckRange);
	sequence1->addChild(CCheckAttackRange);
	sequence1->addChild(CAttackPlayer);

	//hp ��
	HpBarObject = new CGameObject;

	HpBarObject->SetName(L"HpBar Object");
	HpBarObject->FrustumCheck(true);
	HpBarObject->AddComponent(new CTransform);

	HpBarObject->Transform()->SetLocalPos(Vec3(-40000.f, 50.f, 0));
	HpBarObject->Transform()->SetLocalScale(Vec3(50, 8, 1));

	HpBarObject->AddComponent(new CMeshRender);
	Ptr<CTexture> tex = CResMgr::GetInst()->Load<CTexture>(L"monHpBar", L"Texture\\UI\\HpBar.png");

	HpBarObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	HpBarObject->MeshRender()->SetMaterial(pMtrl->Clone());

	HpBarObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, tex.GetPointer());
	// Script ����
	pScene->FindLayer(L"Default")->AddGameObject(HpBarObject);
}

CMonsterScript::~CMonsterScript()
{
}


void CMonsterScript::update()
{
	// �浹ü ��ġ ����

	int a = findNearTarget();
	status->TargetObject = targetObjects[findNearTarget()];

	//// Transform ���� ��ǥ���� ���
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTargetPos = status->TargetObject->Transform()->GetLocalPos();
	Vec3 vDir;

	//// �Ѿ� �浹 Ȯ��
	//vector<CGameObject*> vBobjects = pScene->FindLayer(L"Bullet")->GetObjects();
	//int bb = vBobjects.size();
	//for (int i = 0; i < vBobjects.size(); i++)
	//{
	//	if (vBobjects[i])
	//	{
	//		//Vec3 vBPos = vBobjects[i]->Transform()->GetLocalPos();
	//		//if (vPos.x - 30 <= vBPos.x && vPos.x + 30 >= vBPos.x &&
	//		//	vPos.z - 30 <= vBPos.z && vPos.z + 30 >= vBPos.z)
	//		//{
	//		//	//*vBobjects[i]disable();
	//		//	status->hp -= 40;
	//		//	if (status->hp <= 0)
	//		//		DeleteObject(GetObj());
	//		//}
	//	}
	//}

	if (status->state != MonsterState::M_Die)
	{
		// ���� ���� ����
		Vec2 v1, v2;
		v1.x = vPos.x;
		v1.y = vPos.z;
		v2.x = vTargetPos.x;
		v2.y = vTargetPos.z;

		//float XZdistanceToTarget = sqrt(((vPos.x - vTargetPos.x) * (vPos.x - vTargetPos.x)) + ((vPos.z - vTargetPos.z) * (vPos.z - vTargetPos.z)));
		float XZdistanceToTarget = Vec2::Distance(v1, v2);
		status->distanceToPlayer = XZdistanceToTarget;

		vDir.x = vTargetPos.x - vPos.x;
		vDir.y = 0;
		vDir.z = vTargetPos.z - vPos.z;
		vDir = vDir.Normalize();

		root->run();

		// Hp Bar ��ġ ũ�� ������Ʈ
		Vec3 preVpos = HpBarObject->Transform()->GetLocalPos();
		Vec3 dot1 = XMVector3Dot(Vec3(-1, 0, 0), Vec3(vDir.x, vDir.y, vDir.z));
		float fdot = dot1.x;
		Vec3 dot2 = XMVector3Dot(Vec3(1, 0, 0), Vec3(vDir.x, vDir.y, vDir.z));
		float fdot2 = dot2.x;

		if(vDir.x < 0 && dot1.x > 0.5)
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x - 40) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		else if (vDir.x > 0 && dot2.x > 0.5)
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x + 40) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		else
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		HpBarObject->Transform()->SetLocalScale(Vec3(50 * status->hp / 100. , 10, 1));

		// Ư�� �Ѿ� ȿ�� �ð� ����
		// ����
		if (status->IceTime > 0)
			status->IceTime -= DT;
		else
			status->IceTime = 0;

		// ��
		if (status->FireTime > 0)
		{
			status->FireTime -= DT;
			status->hp -= DT * 10;
		}
		else
			status->FireTime = 0;

		// ����
		if (status->ThunderTime > 0)
		{
			status->ThunderTime -= DT;
			status->hp -= DT * 7;
		}
		else
			status->ThunderTime = 0;

		//if (status->state == MonsterState::M_Run)
		if (status->state == MonsterState::M_Run && !status->IsCollide)
		{
			// ���� �Ѿ� �¾��� ���
			if(status->IceTime > 0)
				vPos += DT * status->speed / 3 * vDir;
			
			else
				vPos += DT * status->speed * vDir;
		}

		// �̰� ���߿� ���º��� ���Եǰ� ����(run, attack??<- �̺κ��� �ٽ� ����)

		// �÷��̾ ���� ���� ������ ���� ��쿡�� ������ ����
		if (XZdistanceToTarget <= status->recognizeRange)
		{
			float temp = atan2(vTargetPos.z - vPos.z, vTargetPos.x - vPos.x);
			Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));
			Transform()->SetLocalPos(vPos);
		}
	}

	// ���� ������ �ִϸ��̼� ����
	if (status->hp <= 0 && status->state != MonsterState::M_Die)
	{
		status->state = MonsterState::M_Die;

		//�ִϸ��̼� ����
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Death.mdat", L"MeshData\\Zombie1Death.mdat");
		pObject->ChangeAnimation(pMeshData);
		Transform()->SetLocalPos(Vec3(vPos.x, vPos.y + 75, vPos.z));
	}

	// ���� ��ü ������� �ð� üũ
	if (status->hp <= 0 && status->state == MonsterState::M_Die)
	{
		status->disappearCnt += DT;
		if (status->disappearCnt > 3.2)
		{
			//DeleteObject(GetObj());
			//pObject->SetDead();
			if (!status->IsDisappear)
			{
				status->IsDisappear = true;

				//������ ����
				bool ItemSpawn = false;
				int rnd = rand() % 100;

				if (rnd < 80)
				{
					for (int i = 0; i < MAX_LAYER; ++i)
					{
						if (!ItemSpawn)
						{
							const vector<CGameObject*>& vecObject = pScene->GetLayer(i)->GetObjects();
							for (size_t j = 0; j < vecObject.size(); ++j)
							{
								if (L"Item Object" == vecObject[j]->GetName() && !ItemSpawn)
								{
									//// ���� ���� ����
									//// �Ŀ� ���� ����
									//if (rnd < 10)
									//{
									//	if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_PwPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
									//	{
									//		vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
									//		vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
									//		vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
									//		ItemSpawn = true;
									//		break;
									//	}
									//}

									//// ��� ���� ����
									//else if (rnd < 20)
									//{
									//	if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_DfPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
									//	{
									//		vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
									//		vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
									//		vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
									//		ItemSpawn = true;
									//		break;
									//	}
									//}

									//// �ӵ� ���� ����
									//else if (rnd < 30)
									//{
									//	if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_SpPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
									//	{
									//		vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
									//		vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
									//		vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
									//		ItemSpawn = true;
									//		break;
									//	}
									//}

									//// �� �� ����
									//else if (rnd < 40)
									//{
									//	if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_HpItem && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
									//	{
									//		vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
									//		vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
									//		vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
									//		vecObject[j]->Transform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
									//		ItemSpawn = true;
									//		break;
									//	}
									//}

									// �ҷ� �ڽ� ����
									if (rnd < 100)
									{
										if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_BulletItem && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
										{
											vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
											vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
											vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
											vecObject[j]->Transform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
											ItemSpawn = true;
											break;
										}
									}
								}
							}
						}
					}
				}
					
			}
		}
	}

	// ���� �� Ÿ�� üũ
	if (status->isAttack)
	{
		status->attackCoolTime -= DT;
		if (status->attackCoolTime <= 0)
		{
			status->isAttack = false;
			status->attackCoolTime = 2.6f;
		}
	}
}

int CMonsterScript::findNearTarget()
{
	Vec3 vPos = Transform()->GetLocalPos();
	int nearNum = -1;
	float distance = 99999999;

	for (int i = 0; i < targetNum; i++)
	{
		Vec3 temp = targetObjects[i]->Transform()->GetLocalPos();
		float tempDistance = sqrt((vPos.x - temp.x) * (vPos.x - temp.x) + (vPos.z - temp.z) * (vPos.z - temp.z));

		if (distance > tempDistance)
		{
			nearNum = i;
			distance = tempDistance;
		}
	}
	return nearNum;
}

void CMonsterScript::OnCollisionEnter(CCollider2D* _pOther)
{
	// �浹�� �߻��ϰ�, ��� ��ü�� �Ѿ��̸� ü�� ����
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{
		CBulletScript* bulletScript = _pOther->GetObj()->GetScript<CBulletScript>();

		if (status->hp >= 0)
		{
			status->hp -= bulletScript->GetDamage();

			if (bulletScript->GetBulletState() == BulletState::B_Fire)
			{
				status->FireTime += 3;
			}

			else if (bulletScript->GetBulletState() == BulletState::B_Ice)
			{
				status->IceTime = 2;
			}

			else if (bulletScript->GetBulletState() == BulletState::B_Thunder)
			{
				status->ThunderTime += 3;

				for (int i = 0; i < MAX_LAYER; ++i)
				{
					const vector<CGameObject*>& vecObject = pScene->GetLayer(i)->GetObjects();
					for (size_t j = 0; j < vecObject.size(); ++j)
					{
						// �̴ϸʿ� �÷��̾� ��ġ ������Ʈ5
						if (L"Monster Object" == vecObject[j]->GetName() || L"Boss Object" == vecObject[j]->GetName())
						{
							Vec3 monsterPos = vecObject[j]->Transform()->GetLocalPos();
							Vec3 Pos = Transform()->GetLocalPos();

							Vec3 sub = Pos - monsterPos;
							float length = sqrt(sub.x * sub.x + sub.y * sub.y + sub.z * sub.z);
							if (length < 200.f)
							{
								if(vecObject[j]->GetName() == L"Monster Object")
									vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->ThunderTime = 3.f;
								else
									vecObject[j]->GetScript<CBossScript>()->GetStatus()->ThunderTime = 3.f;
							}
						}
					}
				}
			}
		}

	}

	if (L"Monster Object" == _pOther->GetObj()->GetName() && !_pOther->GetObj()->GetScript<CMonsterScript>()->status->IsCollide)
	{
		if (!status->state == MonsterState::M_Die && !_pOther->GetObj()->GetScript<CMonsterScript>()->status->state == MonsterState::M_Die)
		{
			status->IsCollide = true;

			Vec3 vPos = Transform()->GetLocalPos();
			Vec3 vDir = Transform()->GetLocalDir(DIR_TYPE::FRONT);

			Vec3 vOtherPos = _pOther->GetObj()->Transform()->GetLocalPos();
			Vec3 vOtherDir = _pOther->GetObj()->Transform()->GetLocalDir(DIR_TYPE::FRONT);

			vPos += 7.f * vDir;
			vDir = Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		}
	}
}

void CMonsterScript::OnCollisionExit(CCollider2D* _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		status->IsCollide = false;
	}
}

void CMonsterScript::OnCollision(CCollider2D* _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName() && !_pOther->GetObj()->GetScript<CMonsterScript>()->status->IsCollide)
	{
		status->IsCollide = true;

		Vec3 vPos = Transform()->GetLocalPos();
		Vec3 vDir = Transform()->GetLocalDir(DIR_TYPE::FRONT);

		Vec3 vOtherPos = _pOther->GetObj()->Transform()->GetLocalPos();
		Vec3 vOtherDir = _pOther->GetObj()->Transform()->GetLocalDir(DIR_TYPE::FRONT);

		vPos += DT * 70.f * vDir;
		vDir = Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		vPos += DT * 100.f * vDir;
		Transform()->SetLocalPos(vPos);

	/*	if (vDir == vOtherDir)
		{
			vOtherPos -= DT * 2.f * vOtherDir;
			_pOther->GetObj()->Transform()->SetLocalPos(vOtherPos);
		}*/
	}
}

void CMonsterScript::SetStatus(MonsterStatus* st)
{
	status->state = st->state;
	status->distanceToPlayer = st->distanceToPlayer;
	status->attackRange = st->attackRange;
	status->PlayerInRange = st->PlayerInRange;
	status->PlayerInAttackRange = st->PlayerInAttackRange;
	status->isAttack = st->isAttack;
	status->hp = st->hp;
	status->disappearCnt = st->disappearCnt;
	status->IsDisappear = st->IsDisappear;
}
