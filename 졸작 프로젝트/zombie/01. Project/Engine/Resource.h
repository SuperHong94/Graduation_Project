#pragma once
#include "Entity.h"


class CResource :
	public CEntity
{
private:
	wstring		m_strPath;	// 상대경로
	int			m_iRefCount;
	RES_TYPE	m_eType;

public:
	RES_TYPE GetType() { return m_eType; }

public:
	CResource(RES_TYPE _eType);
	virtual ~CResource();
};

