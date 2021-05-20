#pragma once
#include "Script.h"
#include "Scene.h"
#include "SceneMgr.h"

struct PlayerStatus
{
	PlayerState state = P_Spawn;
	BulletState bulletState = B_Normal;
	float hp = 100.f;
	float defence = 0.f;
	float AdditionAttack = 0.f; //추가 공격력
	float AdditionSpeed = 0.f; //추가 속도
	float speed = 4000.f;
	//float speed = 300.f;
	float RollCoolTime = 0.f;
	bool IsRoll = false;
	bool IsDead = false;
	float disappearCnt = 0;
	bool isDisappear = false;

	float powerBuffTime = 0.f;
	float defenceBuffTime = 0.f;
	float speedBuffTime = 0.f;
};

class CPlayerScript :
	public CScript
{
private:
	Ptr<CMaterial>		m_pOriginMtrl;
	Ptr<CMaterial>		m_pCloneMtrl;

	float bulletHeight = 0;
	float shiftCoolTime = 1.2f;
	float AddAtk = 10;	// 추가 공격력 수치 (수정시 이걸로)
	float Adddefence = 5;	// 방어력 수치 (수정시 이걸로)
	float AddSpead = 150;  // 추가 이속 수치 (수정시 이걸로)
	Vec3 rollDir;

	CGameObject* pObject;
	PlayerStatus* status;

	PlayerState previousState = P_Spawn; // 플레이어의 이전 프레임 애니메이션 상태

	bool isPlayer;

	CGameObject* pBullet[70];
	int BulletCnt = 70;

	float BulletCollOffset = 0.f;
	float collOffset = 20000;

public:
	virtual void awake();
	virtual void update();

	PlayerState setRunAni(Vec3 dir, Vec3 axis);

	void getDamage(float damage);

	PlayerStatus* GetStatus() { return status; };
	void SetStatus(PlayerStatus* st);

	void SetBulletCollOffset(float offset) { BulletCollOffset = offset; };
	float GetBulletCollOffset() {return BulletCollOffset;};

	float GetCollOffset() { return collOffset; };
	void SetCollOffset(float offset) { collOffset = offset; };

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript(CGameObject* Object, bool player);
	virtual ~CPlayerScript();
};

