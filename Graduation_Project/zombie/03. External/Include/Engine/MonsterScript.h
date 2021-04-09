#pragma once
#include "Script.h"
#include "BehaviourTree.h"


struct MonsterStatus
{
	MonsterState state;
	float distanceToPlayer = 0;
	float attackRange = 100;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
	float hp = 100;
	float disappearCnt = 0;
	bool IsDisappear = false;
	bool IsCollide = false;
};


class CheckPlayerInRange : public Node {
private:

	MonsterStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:

	CheckPlayerInRange(MonsterStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 1000)
		{
			status->PlayerInRange = true;

			//상태 변경
			if (status->state != MonsterState::M_Run && status->distanceToPlayer > status->attackRange)
			{
				status->state = MonsterState::M_Run;

				//애니메이션 변경
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Run.mdat", L"MeshData\\Zombie1Run.mdat");
				pObject->ChangeAnimation(pMeshData);
			}
		}
		else
		{
			status->PlayerInRange = false;
			status->state = MonsterState::M_Wander;
		}
		return status->PlayerInRange;
	}
};

class CheckPlayerInAttackRange : public Node {
private:
	MonsterStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:
	CheckPlayerInAttackRange(MonsterStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= status->attackRange)
		{
			status->PlayerInAttackRange = true;

			//상태변경
			if (status->state != MonsterState::M_Attack)
			{
				status->state = MonsterState::M_Attack;

				//애니메이션 변경
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Attack.mdat", L"MeshData\\Zombie1Attack.mdat");
				pObject->ChangeAnimation(pMeshData);
			}
		}

		else
		{
			status->PlayerInAttackRange = false;
			if (status->distanceToPlayer <= 1000)
			{
				status->state = MonsterState::M_Run;
			}
			else
			{
				status->PlayerInRange = false;
				status->state = MonsterState::M_Wander;
			}
		}
		return status->PlayerInAttackRange;
	}
};

class AttackPlayer : public Node {
private:
	MonsterStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:
	AttackPlayer(MonsterStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 0)
		{

			// 나중에 구현
			status->PlayerInAttackRange = true;
			status->state = MonsterState::M_Attack;
		}
		return true;
	}
};


class CMonsterScript :
	public CScript
{
private:
	MonsterStatus* status;

	Sequence* root;
	Sequence* sequence1;

	CheckPlayerInRange* CCheckRange;
	CheckPlayerInAttackRange* CCheckAttackRange;
	AttackPlayer* CAttackPlayer;

	CGameObject* TargetObejct;
	CGameObject* pObject;
	CScene* pScene;
public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);
	virtual void OnCollision(CCollider2D* _pOther);

	MonsterStatus* GetStatus() { return status; };
	void SetStatus(MonsterStatus* st);
public:
	CLONE(CMonsterScript);

public:
	CMonsterScript(CGameObject* TargetObject, CGameObject* Object, CScene* pscene);
	virtual ~CMonsterScript();

	int aa = 0;
};


