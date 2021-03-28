#pragma once
#include "Script.h"


struct PlayerStatus
{
	State state;
	float hp = 100;
};

class CPlayerScript :
	public CScript
{
private:
	Ptr<CMaterial>		m_pOriginMtrl;
	Ptr<CMaterial>		m_pCloneMtrl;
	float bulletHeight = 0;

	CGameObject* pObject;
	PlayerStatus* status;
	
public:
	virtual void awake();	
	virtual void update();

public:
	CLONE(CPlayerScript);

public:
	CPlayerScript(CGameObject* Object);
	virtual ~CPlayerScript();
};

