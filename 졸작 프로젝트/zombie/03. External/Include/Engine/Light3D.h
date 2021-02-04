#pragma once
#include "Component.h"

class CLight3D :
	public CComponent
{
private:
	tLight3D m_tLightInfo;

public:
	void SetLightType(LIGHT_TYPE _eType);
	void SetLightPos(const Vec3& _vPos);
	void SetDiffuseColor(const Vec3& _vDiffuse) { m_tLightInfo.tColor.vDiff = _vDiffuse; }
	void SetSpecular(const Vec3& _vSpec) { m_tLightInfo.tColor.vSpec = _vSpec; }
	void SetAmbient(const Vec3& _vAmb) { m_tLightInfo.tColor.vAmb = _vAmb; }	
	void SetLightDir(const Vec3& _vDir);
	void SetLightRange(float _fRange) { m_tLightInfo.fRange = _fRange; }
	const tLight3D& GetLight3DInfo() { return m_tLightInfo; }



public:
	virtual void finalupdate();


	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLONE(CLight3D);

public:
	CLight3D();
	virtual ~CLight3D();
};

