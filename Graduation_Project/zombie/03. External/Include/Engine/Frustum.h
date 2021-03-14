#pragma once
#include "Entity.h"

class CCamera;

class CFrustum :
	public CEntity
{
private:
	CCamera*	m_pCam;
	Vec3		m_arrProj[8];
	Vec4		m_arrFace[(UINT)FACE_TYPE::FT_END];
	
public:
	void init();
	void finalupdate();

	bool CheckFrustum(const Vec3& _vPos);
	bool CheckFrustumSphere(const Vec3& _vPos, float _fRadius);

public:
	CFrustum(CCamera* _pCam);
	virtual ~CFrustum();
};

