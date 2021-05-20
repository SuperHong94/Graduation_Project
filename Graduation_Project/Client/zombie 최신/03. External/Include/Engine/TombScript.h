#pragma once
#include "Script.h"

class CTombScript :
	public CScript
{
private:
	int hp = 100;
	bool IsDisappear = false;

public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);
	virtual void OnCollision(CCollider2D* _pOther);

	bool CheckIsDisappear() { return IsDisappear; };
public:
	CLONE(CTombScript);

public:
	CTombScript();
	virtual ~CTombScript();
};

