#include "stdafx.h"
#include "MonsterScript.h"
#include "BulletScript.h"
#include "ItemScript.h"
#include "BossScript.h"
#include "ParticleSystem.h"
#include "TParticleSystem.h"
#include "IParticleSystem.h"
#include "FParticleSystem.h"


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

	//hp 바
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
	// Script 설정
	pScene->FindLayer(L"Default")->AddGameObject(HpBarObject);

	//////////////////////////////////////////////////////////////////
	// 파티클 생성
	FParticleObject = new CGameObject;
	FParticleObject->SetName(L"Particle");
	FParticleObject->AddComponent(new CTransform);
	FParticleObject->AddComponent(new CFParticleSystem);

	FParticleObject->FrustumCheck(true);
	FParticleObject->Transform()->SetLocalPos(Vec3(50.f, 50.f, 0.f));
	pScene->FindLayer(L"Default")->AddGameObject(FParticleObject);
	FParticleObject->SetDead();
	
	//////////////////////////////////////////////////////////////////
	IParticleObject = new CGameObject;
	IParticleObject->SetName(L"Particle");
	IParticleObject->AddComponent(new CTransform);
	IParticleObject->AddComponent(new CIParticleSystem);

	IParticleObject->FrustumCheck(true);
	IParticleObject->Transform()->SetLocalPos(Vec3(50.f, 50.f, 0.f));
	pScene->FindLayer(L"Default")->AddGameObject(IParticleObject);
	IParticleObject->SetDead();
	
	//////////////////////////////////////////////////////////////////
	TParticleObject = new CGameObject;
	TParticleObject->SetName(L"Particle");
	TParticleObject->AddComponent(new CTransform);
	TParticleObject->AddComponent(new CTParticleSystem);

	TParticleObject->FrustumCheck(true);
	TParticleObject->Transform()->SetLocalPos(Vec3(50.f, 50.f, 0.f));
	pScene->FindLayer(L"Default")->AddGameObject(TParticleObject);
	TParticleObject->SetDead();
}

CMonsterScript::~CMonsterScript()
{
}


void CMonsterScript::update()
{
	// 충돌체 위치 설정

	int a = findNearTarget();
	status->TargetObject = targetObjects[findNearTarget()];

	//// Transform 월드 좌표정보 얻기
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTargetPos = status->TargetObject->Transform()->GetLocalPos();
	Vec3 vDir;
	

	//// 총알 충돌 확인
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
	HpBarObject->Transform()->SetLocalScale(Vec3(50 * status->hp / 100., 10, 1));

	if (status->hp > 0)
	{
		// 좀비 방향 설정
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

		// Hp Bar 위치 크기 업데이트
		Vec3 preVpos = HpBarObject->Transform()->GetLocalPos();
		Vec3 dot1 = XMVector3Dot(Vec3(-1, 0, 0), Vec3(vDir.x, vDir.y, vDir.z));
		float fdot = dot1.x;
		Vec3 dot2 = XMVector3Dot(Vec3(1, 0, 0), Vec3(vDir.x, vDir.y, vDir.z));
		float fdot2 = dot2.x;

		if (vDir.x < 0 && dot1.x > 0.5)
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x - 40) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		else if (vDir.x > 0 && dot2.x > 0.5)
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x + 40) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		else
			HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.4) + (vPos.x) * 0.4, vPos.y + 180, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		HpBarObject->Transform()->SetLocalScale(Vec3(50 * status->hp / 100., 10, 1));

		// 특수 총알 효과 시간 차감 & 파티클 효과 설정

		//// 번개 총알은 안 맞아도 주변에서 맞으면 효과가 생김
		//// 그래서 아래 조건으로 파트클 상태 검사해야됨
		//if (status->ThunderTime > 0)
		//	status->ParticleState = 3;

		//if (status->PreParticleState == status->ParticleState)
		//	status->IsParticleChange = false;

		//else
		//	status->IsParticleChange = true;

		////PreParticleState = ParticleState;


		//// 얼음
		//if (status->IceTime > 0)
		//{
		//	particleNum = 1;
		//	status->IceTime -= DT;
		//}
		//else
		//{
		//	//particleNum = -1;
		//	status->IceTime = 0;
		//}
		//// 불
		//if (status->FireTime > 0)
		//{
		//	particleNum = 2;
		//	status->FireTime -= DT;
		//	status->hp -= DT * 10;
		//}
		//else
		//{
		//	//particleNum = -1;
		//	status->FireTime = 0;
		//}
		// 번개
		if (status->ThunderTime > 0)
		{
			particleNum = 3;
			status->ThunderTime -= DT;
			status->hp -= DT * 7;
		}
		//else
		//{
		//	//particleNum = -1;
		//	status->ThunderTime = 0;
		//}


	/*	FParticleObject->SetAlive();
		IParticleObject->SetAlive();*/
		//TParticleObject->SetAlive();

		Vec3 particlePos = Vec3(vPos.x, 100, vPos.z);

		if (!FParticleObject->IsDead() && status->FireTime > 0.f)
			FParticleObject->Transform()->SetLocalPos(particlePos);
	/*	else
			FParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y, particlePos.z));*/

		if (!IParticleObject->IsDead() && status->IceTime > 0.f)
			IParticleObject->Transform()->SetLocalPos(particlePos);
		/*else
			IParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y, particlePos.z));*/

		if (!TParticleObject->IsDead() && status->ThunderTime > 0.f)
		{
			TParticleObject->Transform()->SetLocalPos(particlePos);
		}
		/*else
			TParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y, particlePos.z));*/

		/*FParticleObject->SetAlive();
		FParticleObject->Transform()->SetLocalPos(particlePos);
		IParticleObject->Transform()->SetLocalPos(particlePos);
		TParticleObject->Transform()->SetLocalPos(particlePos);*/
		//FParticleObject->Transform()->SetLocalPos(Vec3(50.f, 50.f, 0.f));

		//if (status->state == MonsterState::M_Run)
		if (status->state == MonsterState::M_Run && !status->IsCollide)
		{
			// 얼음 총알 맞았을 경우
			if (status->IceTime > 0)
				vPos += DT * status->speed / 3 * vDir;

			else
				vPos += DT * status->speed * vDir;
		}




		// 이거 나중에 상태별로 포함되게 수정(run, attack??<- 이부분은 다시 생각)

		// 플레이어가 좀비 인지 범위에 있을 경우에만 움직임 설정
		if (XZdistanceToTarget <= status->recognizeRange)
		{
			float temp = atan2(vTargetPos.z - vPos.z, vTargetPos.x - vPos.x);
			Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));
			Transform()->SetLocalPos(vPos);
		}
	}

	//particleNum = 3;
	if (particleNum == 1)
		IParticleObject->SetAlive();
	else if (particleNum == 2)
		FParticleObject->SetAlive();
	if (particleNum == 3)
	{
		TParticleObject->SetAlive();
		nn++;
		if (nn == 1)
			int c = 3;
	}


	//if (status->IsParticleOn && status->hp > 0 && status->state != MonsterState::M_Die)
	//{
	//	//파티클 위치 업데이트
	//	if (status->FireTime > 0.f)
	//		FParticleObject->Transform()->SetLocalPos(particlePos);
	//	else
	//		FParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y - 1000, particlePos.z));

	//	if (status->IceTime > 0.f)
	//		IParticleObject->Transform()->SetLocalPos(particlePos);
	//	else
	//		IParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y - 1000, particlePos.z));


	//	if (status->ThunderTime > 0.f)
	//		TParticleObject->Transform()->SetLocalPos(particlePos);
	//	else
	//		TParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, particlePos.y - 1000, particlePos.z));
	//
	//	/*if (status->FireTime <= 0.f && FParticleObject != NULL && status->PreParticleState == 1 && !status->IsParticleChange)
	//	{
	//		DeleteObject(FParticleObject);
	//		status->IsParticleOn = false;
	//		status->PreParticleState = -1;
	//		status->ParticleState = -1;
	//	}
	//	else if (status->IceTime <= 0.f && IParticleObject != NULL && status->PreParticleState == 2 && !status->IsParticleChange)
	//	{
	//		DeleteObject(IParticleObject);
	//		status->IsParticleOn = false;
	//		status->PreParticleState = -1;
	//		status->ParticleState = -1;
	//	}
	//	else if (status->ThunderTime <= 0.f && TParticleObject != NULL && status->PreParticleState == 3 && !status->IsParticleChange)
	//	{
	//		DeleteObject(TParticleObject);
	//		status->IsParticleOn = false;
	//		status->PreParticleState = -1;
	//		status->ParticleState = -1;
	//	}*/


	//	////파티클 위치 업데이트
	//	//else if (IsParticleOn)
	//	//	ParticleObject->Transform()->SetLocalPos(particlePos);
	//}

	//파티클 제거
	if (status->hp <= 0 || status->state == MonsterState::M_Die)
	{
	/*	if(!FParticleObject->IsDead())
			FParticleObject->SetDead();
		if (!IParticleObject->IsDead())
			IParticleObject->SetDead();
		if (!TParticleObject->IsDead())
			TParticleObject->SetDead();*/

		//status->PreParticleState = -1;
		//status->ParticleState = -1;
		////ParticleObject->Transform()->SetLocalPos(Vec3(particlePos.x, -1000, particlePos.z));

		////DeleteObject(ParticleObject);
		//status->IsParticleOn = false;
	}

	// 몬스터 죽을시 애니메이션 변경
	if (status->hp <= 0 && status->state != MonsterState::M_Die)
	{
		status->state = MonsterState::M_Die;

		//애니메이션 변경
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Death.mdat", L"MeshData\\Zombie1Death.mdat");
		pObject->ChangeAnimation(pMeshData);
		Transform()->SetLocalPos(Vec3(vPos.x, vPos.y + 75, vPos.z));
	}

	// 몬스터 시체 사라지는 시간 체크
	if (status->hp <= 0 && status->state == MonsterState::M_Die)
	{
		status->disappearCnt += DT;
		if (status->disappearCnt > 2.3)
		{
			//DeleteObject(GetObj());
			//pObject->SetDead();
			if (!status->IsDisappear)
			{
				status->disappearCnt = 0;
				status->IsDisappear = true;

				//아이템 스폰
				bool ItemSpawn = false;
				int rnd = rand() % 100;

				if (rnd < 200)
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
									////// 버프 포션 스폰
									////// 파워 포션 스폰
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

									//// 방어 포션 스폰
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

									//// 속도 포션 스폰
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

									//// 힐 팩 스폰
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

									// 불렛 박스 스폰
									if (rnd < 200)
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

	// 공격 쿨 타임 체크
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
	// 충돌이 발생하고, 상대 물체가 총알이면 체력 감소
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{
		CBulletScript* bulletScript = _pOther->GetObj()->GetScript<CBulletScript>();

		if (status->hp >= 0)
		{
			status->hp -= bulletScript->GetDamage();

			if (bulletScript->GetBulletState() == BulletState::B_Fire)
			{
				status->FireTime += 3;
				status->IceTime = 0;
				status->ThunderTime = 0;
				status->ParticleState = 1;
			}

			else if (bulletScript->GetBulletState() == BulletState::B_Ice)
			{
				status->FireTime = 0;
				status->IceTime = 2;
				status->ThunderTime = 0;
				status->ParticleState = 2;
			}

			else if (bulletScript->GetBulletState() == BulletState::B_Thunder)
			{
				status->FireTime = 0;
				status->IceTime = 0;
				status->ThunderTime += 2;
				status->ParticleState = 3;

				for (int i = 0; i < MAX_LAYER; ++i)
				{
					const vector<CGameObject*>& vecObject = pScene->GetLayer(i)->GetObjects();
					for (size_t j = 0; j < vecObject.size(); ++j)
					{
						// 미니맵에 플레이어 위치 업데이트5
						if (L"Monster Object" == vecObject[j]->GetName() || L"Boss Object" == vecObject[j]->GetName())
						{
							Vec3 monsterPos = vecObject[j]->Transform()->GetLocalPos();
							Vec3 Pos = Transform()->GetLocalPos();

							Vec3 sub = Pos - monsterPos;
							float length = sqrt(sub.x * sub.x + sub.y * sub.y + sub.z * sub.z);
							if (length < 200.f)
							{
								if (vecObject[j]->GetName() == L"Monster Object")
								{
									/*vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->FireTime = 0.f;
									vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->IceTime = 0.f;
									vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->ThunderTime = 1.5f;
									vecObject[j]->GetScript<CMonsterScript>()->SetParticleState(3);*/
								}
								else
								{
									vecObject[j]->GetScript<CBossScript>()->GetStatus()->FireTime = 0.f;
									vecObject[j]->GetScript<CBossScript>()->GetStatus()->IceTime = 0.f;
									vecObject[j]->GetScript<CBossScript>()->GetStatus()->ThunderTime = 1.5f;
									//vecObject[j]->GetScript<CBossScript>()->SetParticleState(3);
								}
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
