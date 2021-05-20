#pragma once
#include "Component.h"

class CLight2D :
	public CComponent
{
private:
	tLight2D   m_LightInfo;
	
public:	
	virtual void finalupdate();

	void UpdateData();

public:
	void SetLightColor(const Vec3& _vColor) { m_LightInfo.vLightColor = _vColor; Changed(); }
	void SetLightRange(float _fRange) { m_LightInfo.fRange = _fRange; Changed();}
	void SetLightType(LIGHT_TYPE _eType) { m_LightInfo.iLightType = (int)_eType; Changed();}

	Vec3 GetLightColor() { return m_LightInfo.vLightColor; }
	float GetLightRange() { return m_LightInfo.fRange; }
	LIGHT_TYPE GetLightType() { return (LIGHT_TYPE)m_LightInfo.iLightType; }

public:
	CLONE(CLight2D);
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLight2D();
	virtual ~CLight2D();
};

