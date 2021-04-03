#include "stdafx.h"
#include "TestScript.h"

CTestScript::CTestScript()
	: CScript((UINT)SCRIPT_TYPE::TESTSCRIPT)
	, m_iLifeCount(2)
{
}

CTestScript::~CTestScript()
{
}

void CTestScript::update()
{

}

void CTestScript::OnCollisionEnter(CCollider2D * _pOther)
{
	// DeleteObject(GetObj());	
	// AddChild(_pOther->GetObj());
	// ClearParent();
}

void CTestScript::OnCollisionExit(CCollider2D * _pOther)
{
	int a = 1;
}
