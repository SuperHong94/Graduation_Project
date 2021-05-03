#pragma once

#include "Script.h"

class CBulletScript :
	public CScript
{
private:
	Vec3 vBulletDir;
	float BulletSpeed = 1000.f;
	float damage = 30;
	BulletState bulletState = BulletState::B_Normal;
	bool BulletActive = false;

public:
	virtual void update();
	virtual void OnCollisionEnter(CCollider2D* _pOther);

public:
	CLONE(CBulletScript);
	virtual BulletState GetBulletState() { return bulletState; };

	float GetDamage() { return damage; };
	bool GetActive() { return BulletActive; };
	void SetActive(bool b) { BulletActive = b; };

	void SetDir(Vec3 d) { vBulletDir = d; };

public:
	CBulletScript(Vec3 Dir, BulletState bState);
	//CBulletScript();
	virtual ~CBulletScript();
};

