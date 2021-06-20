#pragma once
#include "Script.h"
#include "BehaviourTree.h"
#include "PlayerScript.h"


struct MonsterStatus
{
	MonsterState state;
	float distanceToPlayer = 0;
	float attackRange = 100;
	float attackDamage = 10.f;
	float speed = 200.f;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
	float recognizeRange = 1000.f;	// 인지 범위
	float attackCoolTime = 0.7f;
	float attackDelay = 1.2f;	// 정확한 공격 타이밍을 계산하기 위한 변수
	float hp = 100;
	float disappearCnt = 0;
	bool IsDisappear = false;
	bool IsCollide = false;
	CGameObject* TargetObject;

	// 특수 총알 효과 지속 시간
	float IceTime = 0.f;
	float FireTime = 0.f;
	float ThunderTime = 0.f;

	// 파티클
	bool IsFParticleOn = false;
	bool IsTParticleOn = false;
	bool IsIParticleOn = false;
};


class CheckPlayerInRange : public Node {
private:

	MonsterStatus* status;
	CGameObject* pObject;
	CScene* pScene;

public:

	CheckPlayerInRange(MonsterStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= status->recognizeRange)
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

			if (status->state != MonsterState::M_Wander)
			{
				status->state = MonsterState::M_Wander;

				//애니메이션 변경
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Run.mdat", L"MeshData\\Zombie1Run.mdat");
				pObject->ChangeAnimation(pMeshData);
			}
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

			// 공격
			if (!status->isAttack && status->attackDelay <= 0)
			{
				status->isAttack = true;
				{
					status->attackDelay = 1.2f;
					// 플레이어 데미지
					status->TargetObject->GetScript<CPlayerScript>()->getDamage(status->attackDamage);
				}
			}

			else
			{
				status->attackDelay -= DT;
			}
		}

		else
		{
			// 공격 쿨타임 초기화
			status->attackDelay = 1.2f;
			status->attackCoolTime = 2.6f;

			status->PlayerInAttackRange = false;
			if (status->distanceToPlayer <= status->recognizeRange)
			{
				status->state = MonsterState::M_Run;
			}
			else
			{
				status->PlayerInRange = false;
				status->state = MonsterState::M_Wander;
			}

			// 공격 취소
			status->isAttack = false;
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

	CGameObject* pObject;
	CScene* pScene;
	int targetNum;
	CGameObject* targetObjects[4];
	CGameObject* HpBarObject;
	CGameObject* FParticleObject;
	CGameObject* TParticleObject;
	CGameObject* IParticleObject;

public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);
	virtual void OnCollision(CCollider2D* _pOther);

	MonsterStatus* GetStatus() { return status; };
	void SetStatus(MonsterStatus* st);
	int findNearTarget();

	void checkParticle();

public:
	CLONE(CMonsterScript);

public:
	CMonsterScript(CGameObject* TargetObject[], int ntargetNum, CGameObject* Object, CScene* pscene);
	virtual ~CMonsterScript();
};


