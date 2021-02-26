#pragma once
#include "Script.h"
#include "BehaviourTree.h"

enum MonsterState
{
	Wander,
	Run,
	Attack,
	Damage,
	Die,
};

struct MonsterStatus
{
	MonsterState state;
	float distanceToPlayer = 0;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
};


class CheckPlayerInRange : public Node {
private:

	MonsterStatus* status;
public:
	CheckPlayerInRange(MonsterStatus* status) : status(status) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 1000)
		{
			status->PlayerInRange = true;
			status->state = MonsterState::Run;
		}
		else
		{
			status->PlayerInRange = false;
			status->state = MonsterState::Wander;
		}
		return status->PlayerInRange;
	}
};

class CheckPlayerInAttackRange : public Node {
private:
	MonsterStatus* status;
public:
	CheckPlayerInAttackRange(MonsterStatus* status) : status(status) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 50)
		{
			status->PlayerInAttackRange = true;
			status->state = MonsterState::Attack;
		}

		else
		{
			status->PlayerInAttackRange = false;
			if (status->distanceToPlayer <= 1000)
			{
				status->state = MonsterState::Run;
			}
			else
			{
				status->PlayerInRange = false;
				status->state = MonsterState::Wander;
			}
		}
		return status->PlayerInAttackRange;
	}
};

class AttackPlayer : public Node {
private:
	MonsterStatus* status;
public:
	AttackPlayer(MonsterStatus* status) : status(status) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 0)
		{

			// 나중에 구현
			status->PlayerInAttackRange = true;
			status->state = MonsterState::Attack;
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

public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);


public:
	CLONE(CMonsterScript);

public:
	CMonsterScript(CGameObject* Object);
	virtual ~CMonsterScript();
};
