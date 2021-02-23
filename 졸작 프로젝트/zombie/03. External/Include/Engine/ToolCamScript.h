#pragma once
#include "Script.h"

class CToolCamScript :
	public CScript
{
private:
	float		m_fSpeed;
	float		m_fScaleSpeed;
	float		m_fZoomScaleY = 1;
	float		m_fZoomScaleZ = 1;
	float		m_fZoomSpeed = 0.005;

	CGameObject* m_pShootObject;

public:	
	virtual void update();
	void SetShootObject(CGameObject* _pShootObject) { m_pShootObject = _pShootObject; }

public:
	CLONE(CToolCamScript);

public:
	CToolCamScript(CGameObject* _pPlayer);
	virtual ~CToolCamScript();
};

