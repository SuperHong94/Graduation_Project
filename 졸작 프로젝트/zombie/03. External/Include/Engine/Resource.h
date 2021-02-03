#pragma once
#include "Entity.h"


class CResource :
	public CEntity
{
private:
	wstring		m_strPath;	// �����
	int			m_iRefCount;
	RES_TYPE	m_eType;

public:
	RES_TYPE GetType() { return m_eType; }

public:
	void AddRef() { m_iRefCount++; }
	void SubRef() { m_iRefCount--; }

public:
	CResource(RES_TYPE _eType);
	virtual ~CResource();
};

