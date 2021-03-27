#include "stdafx.h"
#include "MonsterScript.h"

CMonsterScript::CMonsterScript(CGameObject* targetObject, CGameObject* Object, CScene* pscene)
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
{
	TargetObejct = targetObject;
	pObject = Object;
	status = new MonsterStatus;
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
	//// Transform ���� ��ǥ���� ���
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTargetPos = TargetObejct->Transform()->GetLocalPos();
	Vec3 vDir;

	float XZdistanceToTarget = sqrt(((vPos.x - vTargetPos.x) * (vPos.x - vTargetPos.x)) + ((vPos.z - vTargetPos.z) * (vPos.z - vTargetPos.z)));
	status->distanceToPlayer = XZdistanceToTarget;

	vDir.x = vTargetPos.x - vPos.x;
	vDir.y = 0;
	vDir.z = vTargetPos.z - vPos.z;
	vDir = vDir.Normalize();

	//if (vPos.x > 600.f)
	//	m_iDir = -1;
	//else if(vPos.x < -600.f)
	//	m_iDir = 1;

	//vPos.x += DT * 100.f * m_iDir;

	// ������ ��ǥ�� �ٽ� �����ϱ�.
	//Transform()->SetLocalPos(vPos);

	root->run();

	if (status->state == MonsterState::Run)
	{
		vPos += DT * 200.f * vDir;
	}

	// �̰� ���߿� ���º��� ���Եǰ� ����(run, attack??<- �̺κ��� �ٽ� ����)
	float temp = atan2(vTargetPos.z - vPos.z, vTargetPos.x - vPos.x);
	Transform()->SetLocalRot(Vec3(0.f, -temp - XM_PI / 2, 0.f));

	Transform()->SetLocalPos(vPos);
}

void CMonsterScript::OnCollisionEnter(CCollider2D* _pOther)
{
	// �浹�� �߻��ϰ�, ��� ��ü�� �Ѿ��̸� ü�� ����
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
		if (status->hp <= 0)
			DeleteObject(GetObj());	// -->���� �̺�Ʈ ���	
	}
}

void CMonsterScript::OnCollisionExit(CCollider2D* _pOther)
{
}
