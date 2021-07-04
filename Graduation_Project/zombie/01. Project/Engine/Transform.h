#pragma once
#include "Component.h"


class CTransform :
	public CComponent
{
private:
	Vec3		m_vLocalPos;	// GameObject 좌표
	Vec3		m_vLocalScale;	// GameObject 크기
	Vec3		m_vLocalRot;	// GameObject 회전량

	Vec3		m_vLocalDir[(UINT)DIR_TYPE::END]; // GameObject 의 Front, Up, Right 방향
	Vec3		m_vWorldDir[(UINT)DIR_TYPE::END]; // GameObject 의 Front, Up, Right 방향

	Matrix		m_matWorld;
	Matrix		m_matWorldInv;

public:
	const Vec3& GetLocalPos() { return m_vLocalPos; }
	Vec3 GetWorldPos() { return m_matWorld.Translation(); }

	const Vec3& GetLocalScale() { return m_vLocalScale; }
	Vec3 GetWorldScale();

	const Vec3& GetLocalRot() { return m_vLocalRot; }
	const Matrix& GetWorldMat() { return m_matWorld; }

	const Vec3& GetLocalDir(DIR_TYPE _eType) { return m_vLocalDir[(UINT)_eType]; }
	const Vec3& GetWorldDir(DIR_TYPE _eType) { return m_vWorldDir[(UINT)_eType]; }

	void SetLocalPos(const Vec3& _vPos) { if (m_vLocalPos != _vPos) { m_vLocalPos = _vPos; Changed(); } }
	void SetLocalScale(const Vec3& _vScale) { if (m_vLocalScale != _vScale) { m_vLocalScale = _vScale; Changed(); } }
	void SetLocalRot(const Vec3& _vRot) { if (m_vLocalRot != _vRot) { m_vLocalRot = _vRot; Changed(); } }

	bool IsCasting(const Vec3& _vPos);
	float GetMaxScale();

	void LookAt(const Vec3& _vLook);

	// Transform 정보를 상수데이터 및 레지스터로 전달한다.
	void UpdateData();

public:
	virtual void finalupdate();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	virtual CTransform* Clone() { return new CTransform(*this); }

public:
	CTransform();
	virtual ~CTransform();
};

