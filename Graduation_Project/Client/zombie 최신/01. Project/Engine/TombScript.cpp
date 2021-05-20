#include "stdafx.h"
#include "TombScript.h"
#include "BulletScript.h"

CTombScript::CTombScript()
	: CScript((UINT)SCRIPT_TYPE::TOMBSCRIPT)
{
}

CTombScript::~CTombScript()
{
}


void CTombScript::update()
{
	if (!IsDisappear)
	{
		if (hp <= 0)
		{
			IsDisappear = true;
			Transform()->SetLocalPos(Vec3(20000.f, 0.f, 0.f));
		}
	}
}

void CTombScript::OnCollisionEnter(CCollider2D* _pOther)
{
	// �浹�� �߻��ϰ�, ��� ��ü�� �Ѿ��̸� ü�� ����
	if (L"Bullet Object" == _pOther->GetObj()->GetName())
	{
		CBulletScript* bulletScript = _pOther->GetObj()->GetScript<CBulletScript>();

		if (hp >= 0)
			hp -= bulletScript->GetDamage();

	}
}

void CTombScript::OnCollisionExit(CCollider2D* _pOther)
{
}

void CTombScript::OnCollision(CCollider2D* _pOther)
{
}
