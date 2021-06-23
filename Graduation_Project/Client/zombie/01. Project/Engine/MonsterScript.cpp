#include "stdafx.h"
#include "MonsterScript.h"
#include "BulletScript.h"
#include "ItemScript.h"

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

		//if (status->state == MonsterState::M_Run)
		if (status->state == MonsterState::M_Run && !status->IsCollide)
		{
			vPos += DT * 200.f * vDir;
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

				if (rnd < 40)
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
									// ���� ���� ����
									// �Ŀ� ���� ����
									if (rnd < 10)
									{
										if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_PwPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
										{
											vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
											vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
											vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
											ItemSpawn = true;
											break;
										}
									}

									// ��� ���� ����
									else if (rnd < 20)
									{
										if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_DfPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
										{
											vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
											vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
											vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
											ItemSpawn = true;
											break;
										}
									}

									// �ӵ� ���� ����
									else if (rnd < 30)
									{
										if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_SpPotion && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
										{
											vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
											vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
											vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
											ItemSpawn = true;
											break;
										}
									}

									// �� �� ����
									else if (rnd < 40)
									{
										if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_HpItem && !vecObject[j]->GetScript<CItemScript>()->getIsSpawn())
										{
											vecObject[j]->GetScript<CItemScript>()->setIsSpawn(true);
											vecObject[j]->GetScript<CItemScript>()->setActiveTime(10.f);
											vecObject[j]->Transform()->SetLocalPos(Vec3(vPos.x, 50.f, vPos.z));
											vecObject[j]->Transform()->SetLocalScale(Vec3(0.2f, 0.2f, 0.2f));
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
			status->hp -= bulletScript->GetDamage();

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
