#pragma once
#include "Script.h"
#include "Scene.h"


struct PlayerStatus
{
	PlayerState state = P_Spawn;
	BulletState bulletState = B_Normal;
	float hp = 100.f;
	//float speed = 4000.f;
	float speed = 300.f;
	float RollCoolTime = 0.f;
	bool IsRoll = false;
	bool IsDead = false;
	float disappearCnt = 0;
	bool isDisappear = false;
};

class CPlayerScript :
	public CScript
{
private:
	Ptr<CMaterial>		m_pOriginMtrl;
	Ptr<CMaterial>		m_pCloneMtrl;

	float bulletHeight = 0;
	float shiftCoolTime = 1.2f;

	Vec3 rollDir;

	CGameObject* pObject;
	PlayerStatus* status;

	PlayerState previousState = P_Spawn; // 플레이어의 이전 프레임 애니메이션 상태

	bool isPlayer;

	CGameObject* pBullet[200];
	int BulletCnt = 200;

	float BulletCollOffset = 0.f;

public:
	virtual void awake();
	virtual void update();

	PlayerState setRunAni(Vec3 dir, Vec3 axis);

	void getDamage(float damage);

	PlayerStatus* GetStatus() { return status; };
	void SetStatus(PlayerStatus* st);

	void SetBulletCollOffset(float offset) { BulletCollOffset = offset; };
	float GetBulletCollOffset() {return BulletCollOffset;};

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript(CGameObject* Object, bool player);
	virtual ~CPlayerScript();
};

