#pragma once
#include "Script.h"
#include "BehaviourTree.h"
#include "Animator3D.h"

enum MonsterState
{
	M_Wander,
	M_Run,
	M_Attack,
	M_Damage,
	M_Die,
};

struct MonsterStatus
{
	MonsterState state;
	float distanceToPlayer = 0;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
	float hp = 100;
};


class CheckPlayerInRange : public Node {
private:

	MonsterStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:
	CheckPlayerInRange(MonsterStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene){}
	virtual bool run() override {
		if (status->distanceToPlayer <= 1000)
		{
			status->PlayerInRange = true;
			status->state = MonsterState::M_Run;

			//애니메이션 변경
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Run.mdat", L"MeshData\\Zombie1Run.mdat");
			pObject->ChangeAnimation(pMeshData);
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
		if (status->distanceToPlayer <= 100)
		{
			status->PlayerInAttackRange = true;
			status->state = MonsterState::M_Attack;

			//애니메이션 변경
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Attack.mdat", L"MeshData\\Zombie1Attack.mdat");
			pObject->ChangeAnimation(pMeshData);
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


public:
	CLONE(CMonsterScript);

public:
	CMonsterScript(CGameObject* TargetObject, CGameObject* Object, CScene* pscene);
	virtual ~CMonsterScript();
};


