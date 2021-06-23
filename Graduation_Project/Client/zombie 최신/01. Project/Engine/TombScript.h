#pragma once
#include "Script.h"

class CTombScript :
	public CScript
{
private:
	float hp = 100;
	bool IsDisappear = false;

public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);
	virtual void OnCollision(CCollider2D* _pOther);

	bool CheckIsDisappear() { return IsDisappear; };
public:
	CLONE(CTombScript);

	float getHp() { return hp; };
	void setHp(float n) { hp = n; };

public:
	CTombScript();
	virtual ~CTombScript();
};

