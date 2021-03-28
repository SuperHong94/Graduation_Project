#pragma once

#include "Script.h"

class CBulletScript :
	public CScript
{
private:
	Vec3 vBulletDir;

public:
	virtual void update();
	virtual void OnCollisionEnter(CCollider2D* _pOther);


public:
	CLONE(CBulletScript);

public:
	CBulletScript(Vec3 Dir, float d);
	//CBulletScript();
	virtual ~CBulletScript();
};

