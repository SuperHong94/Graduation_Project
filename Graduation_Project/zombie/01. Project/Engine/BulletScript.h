#pragma once

#include "Script.h"

class CBulletScript :
	public CScript
{
private:
	Vec3 vBulletDir;
	float BulletSpeed = 1000.f;

public:
	virtual void update();
	virtual void OnCollisionEnter(CCollider2D* _pOther);

public:
	CLONE(CBulletScript);
	virtual BulletState GetBulletState() { return bulletState; };

public:
	CBulletScript(Vec3 Dir, BulletState bState);
	//CBulletScript();
	virtual ~CBulletScript();
};

