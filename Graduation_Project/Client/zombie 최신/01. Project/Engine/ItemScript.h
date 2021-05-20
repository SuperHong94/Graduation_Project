#pragma once

#include "Script.h"

class CItemScript :
	public CScript
{
private:
	ItemState state;
	float activeTime;
	float isSpawn;

public:
	virtual void update();
	virtual void OnCollisionEnter(CCollider2D* _pOther);

	ItemState getState() { return state; };
	void setState(ItemState st) { state = st; };

	float getIsSpawn() { return isSpawn; };
	void setIsSpawn(float Sp) { isSpawn = Sp; };

	float getActiveTime() { return activeTime; };
	void setActiveTime(float time) { activeTime = time; };

public:
	CLONE(CItemScript);

public:
	CItemScript(ItemState State);
	virtual ~CItemScript();
};
