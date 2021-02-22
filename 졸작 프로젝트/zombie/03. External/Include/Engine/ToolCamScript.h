#pragma once
#include "Script.h"

class CToolCamScript :
	public CScript
{
private:
	float		m_fSpeed;
	float		m_fScaleSpeed;
	CGameObject* m_pPlayer;

public:	
	virtual void update();
	void SetPlayer(CGameObject* _pPlayer) { m_pPlayer = _pPlayer; }

public:
	CLONE(CToolCamScript);

public:
	CToolCamScript(CGameObject* _pPlayer);
	virtual ~CToolCamScript();
};

