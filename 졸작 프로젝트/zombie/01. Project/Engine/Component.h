#pragma once
#include "Entity.h"

#include "GameObject.h"

class CComponent :
	public CEntity
{
private:
	COMPONENT_TYPE	m_eType;
	CGameObject*	m_pObj;

public:
	COMPONENT_TYPE GetComType() { return m_eType; }

	CTransform* Transform() { return m_pObj->Transform(); }
	CMeshRender* MeshRender() { return m_pObj->MeshRender(); }

private:
	void SetGameObject(CGameObject* _pObj) { m_pObj = _pObj; }

public:
	virtual void awake() {}
	virtual void start() {}
	virtual void update(){}
	virtual void lateupdate() {}
	virtual void finalupdate() {}


public:
	CComponent(COMPONENT_TYPE _eType)
		: m_eType(_eType)
	{}
	virtual ~CComponent();

	friend class CGameObject;
};

