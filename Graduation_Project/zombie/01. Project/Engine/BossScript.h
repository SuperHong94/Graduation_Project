#pragma once

#include "Script.h"
#include "BehaviourTree.h"
#include "PlayerScript.h"

struct BossStatus
{
	BossState state;
	float distanceToPlayer = 0;
	float attackRange = 100;
	float attackDamage = 30.f;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
	float recognizeRange = 1000.f;	// ���� ����
	float attackCoolTime = 4.3f;
	float attackDelay = 0.8f;	// ��Ȯ�� ���� Ÿ�̹��� ����ϱ� ���� ����
	bool IsAnimationChange = false;
	float checkAnimationChangeTime = 0.3f;
	float hp = 1000;
	float speed = 300;
	float disappearCnt = 0;
	bool IsDisappear = false;
	bool IsCollide = false;
	CGameObject* TargetObject;

	// Ư�� �Ѿ� ȿ�� ���� �ð�
	float IceTime = 0.f;
	float FireTime = 0.f;
	float ThunderTime = 0.f;

	// ��ƼŬ
	bool IsFParticleOn = false;
	bool IsTParticleOn = false;
	bool IsIParticleOn = false;
};

class BossCheckPlayerInRange : public Node {
private:

	BossStatus* status;
	CGameObject* pObject;
	CScene* pScene;

public:

	BossCheckPlayerInRange(BossStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= status->recognizeRange)
		{
			status->PlayerInRange = true;

			//���� ����
			if (status->state != BossState::B_Run && status->distanceToPlayer > status->attackRange && !status->IsAnimationChange)
			{
				status->state = BossState::B_Run;
				status->IsAnimationChange = true;
				//�ִϸ��̼� ����
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\BossRun.mdat", L"MeshData\\BossRun.mdat");
				pObject->ChangeAnimation(pMeshData);
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0, 0));
			}
		}
		else
		{
			status->PlayerInRange = false;

			if (status->state != BossState::B_IDLE && !status->IsAnimationChange)
			{
				status->state = BossState::B_IDLE;
				status->IsAnimationChange = true;
				//�ִϸ��̼� ����
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\BossIdle.mdat", L"MeshData\\BossIdle.mdat");
				pObject->ChangeAnimation(pMeshData);
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0, 0));
			}
		}
		return status->PlayerInRange;
	}
};

class BossCheckPlayerInAttackRange : public Node {
private:
	BossStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:
	BossCheckPlayerInAttackRange(BossStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= status->attackRange)
		{
			status->PlayerInAttackRange = true;

			//���º���
			if (status->state != BossState::B_Attack && !status->IsAnimationChange)
			{
				status->state = BossState::B_Attack;
				status->IsAnimationChange = true;
				//�ִϸ��̼� ����
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\BossAttack.mdat", L"MeshData\\BossAttack.mdat");
				pObject->ChangeAnimation(pMeshData);
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0, 0));
			}

			// ����
			if (!status->isAttack && status->attackDelay <= 0)
			{
				status->isAttack = true;
				{
					status->attackDelay = 0.8f;
					// �÷��̾� ������
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
			// ���� ��Ÿ�� �ʱ�ȭ
			status->attackDelay = 0.8f;
			status->attackCoolTime = 4.3f;

			status->PlayerInAttackRange = false;
			if (!status->IsAnimationChange)
			{
				if (status->distanceToPlayer <= status->recognizeRange)
				{
					status->state = BossState::B_Run;
					status->IsAnimationChange = true;
				}
				else
				{
					status->PlayerInRange = false;
					status->state = BossState::B_IDLE;
					status->IsAnimationChange = true;
				}
			}
			// ���� ���
			status->isAttack = false;
		}
		return status->PlayerInAttackRange;
	}
};

class BossAttackPlayer : public Node {
private:
	BossStatus* status;
	CGameObject* pObject;
	CScene* pScene;
public:
	BossAttackPlayer(BossStatus* status, CGameObject* pObject, CScene* pscene) : status(status), pObject(pObject), pScene(pscene) {}
	virtual bool run() override {
		if (status->distanceToPlayer <= 0 && !status->IsAnimationChange)
		{

			// ���߿� ����
			status->PlayerInAttackRange = true;
			status->state = BossState::B_Attack;
			status->IsAnimationChange = true;
		}
		return true;
	}
};


class CBossScript
	:public CScript
{
	BossStatus* status;

	Sequence* root;
	Sequence* sequence1;

	BossCheckPlayerInRange* CCheckRange;
	BossCheckPlayerInAttackRange* CCheckAttackRange;
	BossAttackPlayer* CAttackPlayer;

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

	BossStatus* GetStatus() { return status; };
	void SetStatus(BossStatus* st);
	int findNearTarget();

	void checkParticle();

public:
	CLONE(CBossScript);

public:
	CBossScript(CGameObject* TargetObject[], int ntargetNum, CGameObject* Object, CScene* pscene);
	virtual ~CBossScript();
};

