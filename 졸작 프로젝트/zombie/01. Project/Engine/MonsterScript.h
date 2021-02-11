#pragma once
#include "Script.h"

class CMonsterScript :
	public CScript
{
private:
	int		m_iDir;

public:
	virtual void update();

	virtual void OnCollisionEnter(CCollider2D* _pOther);
	virtual void OnCollisionExit(CCollider2D* _pOther);
public:
	CLONE(CMonsterScript);

public:
	CMonsterScript();
	virtual ~CMonsterScript();
};

