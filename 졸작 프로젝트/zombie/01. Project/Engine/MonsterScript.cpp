#include "stdafx.h"
#include "MonsterScript.h"

CMonsterScript::CMonsterScript(CGameObject* Object)
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
{
	TargetObejct = Object;
	status = new MonsterStatus;

	root = new Sequence;
	sequence1 = new Sequence;
	CCheckRange = new CheckPlayerInRange(status);
	CCheckAttackRange = new CheckPlayerInAttackRange(status);
	CAttackPlayer = new AttackPlayer(status);

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
	//// Transform 월드 좌표정보 얻기
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTargetPos = TargetObejct->Transform()->GetLocalPos();
	Vec3 vDir;

	float XZdistanceToTarget = sqrt(((vPos.x - vTargetPos.x) * (vPos.x - vTargetPos.x)) + ((vPos.z - vTargetPos.z) * (vPos.z - vTargetPos.z)));
	status->distanceToPlayer = XZdistanceToTarget;

	vDir.x = vTargetPos.x - vPos.x;
	vDir.y = 0;
	vDir.z = vTargetPos.z - vPos.z;

	//if (vPos.x > 600.f)
	//	m_iDir = -1;
	//else if(vPos.x < -600.f)
	//	m_iDir = 1;

	//vPos.x += DT * 100.f * m_iDir;

	// 수정된 좌표를 다시 세팅하기.
	//Transform()->SetLocalPos(vPos);

	root->run();

	if (status->state == MonsterState::Run)
	{
		vPos += DT * 0.3f * vDir;
	}

	Transform()->SetLocalPos(vPos);
}

void CMonsterScript::OnCollisionEnter(CCollider2D * _pOther)
{
	// 충돌이 발생하고, 상대 물체가 총일이면 스스로를 삭제
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{	
		float damage = 0;
		vector<CScript*>  m_vecScript = _pOther->GetObj()->GetScripts();
		for (int i = 0; i < m_vecScript.size(); i++)
		{
			if (L"BulletScript" == m_vecScript[i]->GetName())
			{
				damage = m_vecScript[i]->getDamage();
			}
		}

		status->hp -= damage;
		if(status->hp <= 0)
			DeleteObject(GetObj());	// -->삭제 이벤트 등록	
	}	
}

void CMonsterScript::OnCollisionExit(CCollider2D * _pOther)
{	
}
