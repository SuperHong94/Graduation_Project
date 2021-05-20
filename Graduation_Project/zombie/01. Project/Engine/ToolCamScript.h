#pragma once
#include "Script.h"

class CToolCamScript :
	public CScript
{
private:
	float		m_fSpeed;
	float		m_fScaleSpeed;
	float		m_fZoomScaleY = 13;
	float		m_fZoomScaleZ = 13;
	//float		m_fZoomScaleY = 1.5;
	//float		m_fZoomScaleZ = 1.5;
	float		m_fZoomSpeed = 0.005;

	CGameObject* m_pShootObject;

	Vec3 vPreviousPlayerPos = Vec3(0.f, 0.f, 0.f);
public:
	virtual void update();
	void SetShootObject(CGameObject* _pShootObject) { m_pShootObject = _pShootObject; }

public:
	CLONE(CToolCamScript);

public:
	CToolCamScript(CGameObject* _pPlayer);
	virtual ~CToolCamScript();
};
