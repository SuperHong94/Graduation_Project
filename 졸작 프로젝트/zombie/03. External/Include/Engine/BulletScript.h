#pragma once

#include "Script.h"

class CBulletScript :
	public CScript
{
public:
	virtual void update();
	virtual void OnCollisionEnter(CCollider2D* _pOther);


public:
	CLONE(CBulletScript);

public:
	CBulletScript();
	virtual ~CBulletScript();
};

