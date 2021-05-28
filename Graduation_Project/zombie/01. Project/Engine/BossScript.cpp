#include "stdafx.h"
#include "BossScript.h"
#include "BulletScript.h"


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

	if (status->state != BossState::B_Die)
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

		//if (status->state == MonsterState::M_Run)
		if (status->state == BossState::B_Run && !status->IsCollide)
		{
			vPos += DT * 200.f * vDir;
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
			status->attackCoolTime = 2.6f;
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
}
