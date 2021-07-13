#pragma once
#include "Object.h"
#include "PlayerObject.h"
enum class ZOMBIETYPE
{

};
class CZombie :protected CObject
{
	MonsterState m_eMosterState;
	float distanceToPlayer = 0;
	float attackRange = 100;
	float attackDamage = 10.f;
	float speed = 200.f;
	bool PlayerInRange = false;
	bool PlayerInAttackRange = false;
	bool isAttack = false;
	float recognizeRange = 1000.f;	// ���� ����
	float attackCoolTime = 0.7f;
	float attackDelay = 1.2f;	// ��Ȯ�� ���� Ÿ�̹��� ����ϱ� ���� ����
	float hp = 100;
	float disappearCnt = 0;
	bool IsDisappear = false;
	bool IsCollide = false;
	bool isActive = false;

	CPlayerObject* targetObject;

	int m_id;

public:
	CZombie();
	virtual ~CZombie();
	void init(int m_id);


};

