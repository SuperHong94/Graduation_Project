#pragma once
#include "Component.h"


class CTransform :
	public CComponent
{
private:
	Vec3	m_vLocalPos;
	Vec3	m_vLocalRot;
	Vec3	m_vLocalScale;

	Matrix	m_matWorld;


public:
	void SetLocalPos(const Vec3& _vPos) { m_vLocalPos = _vPos; }
	const Vec3& GetLocalPos() { return m_vLocalPos; }


public:
	virtual void finalupdate();

	// Transform 정보를 상수데이터 및 레지스터로 전달한다.
	void UpdateData();

public:
	CTransform();
	virtual ~CTransform();
};

