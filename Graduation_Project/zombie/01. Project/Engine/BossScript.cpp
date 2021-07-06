#include "stdafx.h"
#include "BossScript.h"
#include "BulletScript.h"
#include "MonsterScript.h"
#include "ParticleSystem.h"
#include "TParticleSystem.h"
#include "IParticleSystem.h"
#include "FParticleSystem.h"

CBossScript::CBossScript(CGameObject* targetObject[], int ntargetNum, CGameObject* Object, CScene* pscene)
	: CScript((UINT)SCRIPT_TYPE::BOSSSCRIPT)
{
	targetNum = ntargetNum;
	for (int i = 0; i < targetNum; i++)
	{
		targetObjects[i] = new CGameObject;
		targetObjects[i] = targetObject[i];
	}

	this->SetName(L"MonsterScript");
	pObject = Object;
	status = new BossStatus;
	status->state = BossState::B_IDLE;
	status->TargetObject = targetObject[0];
	pScene = pscene;

	root = new Sequence;
	sequence1 = new Sequence;
	CCheckRange = new BossCheckPlayerInRange(status, pObject, pScene);
	CCheckAttackRange = new BossCheckPlayerInAttackRange(status, pObject, pScene);
	CAttackPlayer = new BossAttackPlayer(status, pObject, pScene);

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
	HpBarObject->Transform()->SetLocalScale(Vec3(100, 8, 1));

	HpBarObject->AddComponent(new CMeshRender);
	Ptr<CTexture> tex = CResMgr::GetInst()->Load<CTexture>(L"BossHpBar", L"Texture\\UI\\HpBar.png");

	HpBarObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	HpBarObject->MeshRender()->SetMaterial(pMtrl->Clone());

	HpBarObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, tex.GetPointer());
	// Script 설정
	pScene->FindLayer(L"Default")->AddGameObject(HpBarObject);
}

CBossScript::~CBossScript()
{
}


void CBossScript::update()
{
	// 충돌체 위치 설정
	status->TargetObject = targetObjects[findNearTarget()];

	//// Transform 월드 좌표정보 얻기
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTargetPos = status->TargetObject->Transform()->GetLocalPos();
	Vec3 vDir;

	/////////////////////////////////////////////////////
	// 특수 총알 효과 시간 차감 & 파티클 효과 설정
	checkParticle();
	/////////////////////////////////////////////////////

	if (status->state != BossState::B_Die)
	{
		// 애니메이션 체크
		if (status->IsAnimationChange)
		{
			if (status->checkAnimationChangeTime > 0)
				status->checkAnimationChangeTime -= DT;

			if (status->checkAnimationChangeTime <= 0)
			{
				status->checkAnimationChangeTime = 0.3f;
				status->IsAnimationChange = false;
			}
		}

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

		if (HpBarObject->Transform()->GetLocalPos().x < -10000 && vPos.x > -500 && vPos.x < 500)
			HpBarObject->Transform()->SetLocalPos(vPos);

		else
		{
			if (vDir.x < 0 && dot1.x > 0.5)
				HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.2) + (vPos.x - 30) * 0.2, vPos.y + 230, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
			else if (vDir.x > 0 && dot2.x > 0.5)
				HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.2) + (vPos.x + 30) * 0.2, vPos.y + 230, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
			else
				HpBarObject->Transform()->SetLocalPos(Vec3(preVpos.x * (1 - 0.2) + (vPos.x) * 0.2, vPos.y + 230, preVpos.z * (1 - 0.4) + (vPos.z) * 0.4));
		}

		HpBarObject->Transform()->SetLocalScale(Vec3(100 * status->hp / 1000, 10, 1));

		// 특수 총알 효과 시간 차감
		// 얼음
		if (status->IceTime > 0)
			status->IceTime -= DT;
		else
			status->IceTime = 0;

		// 불
		if (status->FireTime > 0)
		{
			status->FireTime -= DT;
			status->hp -= DT * 10;
		}
		else
			status->FireTime = 0;

		// 번개
		if (status->ThunderTime > 0)
		{
			status->ThunderTime -= DT;
			status->hp -= DT * 7;
		}
		/*else
			status->ThunderTime = 0;*/

		//if (status->state == MonsterState::M_Run)
		if (status->state == BossState::B_Run && !status->IsCollide)
		{
			// 얼음 총알 맞았을 경우
			if (status->IceTime > 0)
				vPos += DT * status->speed / 1.5 * vDir;

			else
				vPos += DT * status->speed * vDir;
		}

		// 이거 나중에 상태별로 포함되게 수정(run, attack??<- 이부분은 다시 생각)

		// 플레이어가 좀비 인지 범위에 있을 경우에만 움직임 설정
		if (XZdistanceToTarget <= status->recognizeRange)
		{
			float temp = atan2(vTargetPos.z - vPos.z, vTargetPos.x - vPos.x);
			if(BossState::B_Run && !status->isAttack)
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -temp - XM_PI / 2, 0));
			Transform()->SetLocalPos(vPos);
		}
	}

	// 몬스터 죽을시 애니메이션 변경
	if (status->hp <= 0 && status->state != BossState::B_Die)
	{
		status->state = BossState::B_Die;

		//애니메이션 변경
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\BossDeath.mdat", L"MeshData\\BossDeath.mdat");
		pObject->ChangeAnimation(pMeshData);
		Transform()->SetLocalPos(Vec3(vPos.x, vPos.y, vPos.z));
	}

	// 몬스터 시체 사라지는 시간 체크
	if (status->hp <= 0 && status->state == BossState::B_Die)
	{
		status->disappearCnt += DT;
		if (status->disappearCnt > 3.2)
		{
			//DeleteObject(GetObj());
			//pObject->SetDead();
			if (!status->IsDisappear)
			{
				status->IsDisappear = true;
				Transform()->SetLocalPos(Vec3(20000.f, 20000.f, 20000.f));
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
			status->attackCoolTime = 4.3f;
		}
	}
}

int CBossScript::findNearTarget()
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

void CBossScript::OnCollisionEnter(CCollider2D* _pOther)
{
	// 충돌이 발생하고, 상대 물체가 총알이면 체력 감소
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{
		CBulletScript* bulletScript = _pOther->GetObj()->GetScript<CBulletScript>();

		if (status->hp >= 0)
			status->hp -= bulletScript->GetDamage();

		if (bulletScript->GetBulletState() == BulletState::B_Fire)
		{
			status->FireTime = 2;
			status->IceTime = 0;
			status->ThunderTime = 0;
		}

		else if (bulletScript->GetBulletState() == BulletState::B_Ice)
		{
			status->FireTime = 0;
			status->IceTime = 1;
			status->ThunderTime = 0;
		}

		else if (bulletScript->GetBulletState() == BulletState::B_Thunder)
		{
			status->FireTime = 0;
			status->IceTime = 0;
			status->ThunderTime = 1.5;

			for (int i = 0; i < MAX_LAYER; ++i)
			{
				const vector<CGameObject*>& vecObject = pScene->GetLayer(i)->GetObjects();
				for (size_t j = 0; j < vecObject.size(); ++j)
				{
					// 미니맵에 플레이어 위치 업데이트
					if (L"Monster Object" == vecObject[j]->GetName())
					{
						Vec3 monsterPos = vecObject[j]->Transform()->GetLocalPos();
						Vec3 Pos = Transform()->GetLocalPos();

						Vec3 sub = Pos - monsterPos;
						float length = sqrt(sub.x * sub.x + sub.y * sub.y + sub.z * sub.z);
						if (length < 200.f)
						{
							vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->FireTime = 0.f;
							vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->IceTime = 0.f;
							vecObject[j]->GetScript<CMonsterScript>()->GetStatus()->ThunderTime = 1.5f;
						}
					}
				}
			}
		}
	}

}

void CBossScript::OnCollisionExit(CCollider2D* _pOther)
{
}

void CBossScript::OnCollision(CCollider2D* _pOther)
{
}

void CBossScript::SetStatus(BossStatus* st)
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
	status->IceTime = st->IceTime;
	status->FireTime = st->FireTime;
	status->ThunderTime = st->ThunderTime;
}

void CBossScript::checkParticle()
{
	Vec3 mPos = Transform()->GetLocalPos();
	Vec3 particlePos = Vec3(mPos.x, 150, mPos.z);

	// 얼음
	if (status->IceTime > 0)
	{
		if (!status->IsIParticleOn)
		{
			IParticleObject = new CGameObject;
			IParticleObject->SetName(L"IParticle");
			IParticleObject->AddComponent(new CTransform);
			IParticleObject->AddComponent(new CIParticleSystem);

			IParticleObject->FrustumCheck(true);
			IParticleObject->Transform()->SetLocalPos(Vec3(particlePos));
			pScene->FindLayer(L"Default")->AddGameObject(IParticleObject);

			status->IsIParticleOn = true;
		}
		status->IceTime -= DT;
	}
	else if (status->IceTime <= 0)
	{
		status->IceTime = 0;
		if (status->IsIParticleOn)
		{
			IParticleObject->SetDead();
			status->IsIParticleOn = false;
		}
	}

	// 불
	if (status->FireTime > 0)
	{
		if (!status->IsFParticleOn)
		{
			FParticleObject = new CGameObject;
			FParticleObject->SetName(L"TParticle");
			FParticleObject->AddComponent(new CTransform);
			FParticleObject->AddComponent(new CFParticleSystem);

			FParticleObject->FrustumCheck(true);
			FParticleObject->Transform()->SetLocalPos(particlePos);
			pScene->FindLayer(L"Default")->AddGameObject(FParticleObject);

			status->IsFParticleOn = true;
		}
		status->FireTime -= DT;
		status->hp -= DT * 10;
	}
	else if (status->FireTime <= 0)
	{
		status->FireTime = 0;
		if (status->IsFParticleOn)
		{
			FParticleObject->SetDead();
			status->IsFParticleOn = false;
		}
	}

	// 번개
	if (status->ThunderTime > 0.f)
	{
		if (!status->IsTParticleOn)
		{
			TParticleObject = new CGameObject;
			TParticleObject->SetName(L"TParticle");
			TParticleObject->AddComponent(new CTransform);
			TParticleObject->AddComponent(new CTParticleSystem);

			TParticleObject->FrustumCheck(true);
			TParticleObject->Transform()->SetLocalPos(particlePos);
			pScene->FindLayer(L"Default")->AddGameObject(TParticleObject);

			status->IsTParticleOn = true;
		}
		status->ThunderTime -= DT;
		status->hp -= DT * 7;
	}

	else if (status->ThunderTime <= 0.f)
	{
		status->ThunderTime = 0;
		if (status->IsTParticleOn)
		{
			TParticleObject->SetDead();
			status->IsTParticleOn = false;
		}
	}


	if (status->IsFParticleOn)
		FParticleObject->Transform()->SetLocalPos(particlePos);

	if (status->IsIParticleOn)
		IParticleObject->Transform()->SetLocalPos(particlePos);

	if (status->IsTParticleOn)
		TParticleObject->Transform()->SetLocalPos(particlePos);

	// 죽었을 때 파티클 제거
	if (status->hp <= 0)
	{
		if (status->IsFParticleOn)
		{
			FParticleObject->SetDead();
			status->FireTime = 0;
			status->IsFParticleOn = false;
		}

		if (status->IsIParticleOn)
		{
			IParticleObject->SetDead();
			status->IceTime = 0;
			status->IsIParticleOn = false;
		}

		if (status->IsTParticleOn)
		{
			TParticleObject->SetDead();
			status->ThunderTime = 0;
			status->IsTParticleOn = false;
		}
	}
}