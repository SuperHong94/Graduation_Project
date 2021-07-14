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
	float recognizeRange = 1000.f;	// 인지 범위
	float attackCoolTime = 0.7f;
	float attackDelay = 1.2f;	// 정확한 공격 타이밍을 계산하기 위한 변수
	float hp = 100;
	float disappearCnt = 0;
	bool IsDisappear = false;
	bool IsCollide = false;
	bool isActive = false;

	CPlayerObject* targetObject;
	Vec3 targetDir;
	int m_id;
	chrono::high_resolution_clock::time_point m_last_move_time; //마지막으로 움직인 시각

public:
	CZombie();
	virtual ~CZombie();
	void init(int m_id);
	chrono::high_resolution_clock::time_point getLastTime();
	void move2target(); //타겟 위치로 이동
	void setLastTime(high_resolution_clock::time_point tp);
};

