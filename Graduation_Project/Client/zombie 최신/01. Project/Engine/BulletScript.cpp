#include "stdafx.h"
#include "BulletScript.h"


CBulletScript::CBulletScript(Vec3 Dir, BulletState bState)
	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
{
	this->SetName(L"BulletScript");

	vBulletDir = Dir;
	bulletState = bState;
}

//CBulletScript::CBulletScript()	
//	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
//{
//}

CBulletScript::~CBulletScript()
{
}

void CBulletScript::update()
{
	if (BulletActive)
	{
		switch (bulletState)
		{
		case B_Normal:
			damage = 30;
			break;
		case B_Fire:
			damage = 30;
			break;
		case B_Ice:
			damage = 30;
			break;
		case B_Thunder:
			damage = 30;
			break;
		default:
			damage = 30;
			break;
		}

		damage += AddDamage;

		Vec3 vPos = Transform()->GetLocalPos();
		//vPos.y += 400.f * DT;
		vPos += vBulletDir * BulletSpeed * DT;
		Transform()->SetLocalPos(vPos);

		if (vPos.x >= 5300 || vPos.x <= -5300 || vPos.z >= 5300 || vPos.z <= -5300)
		{
			//DeleteObject(GetObj());
			BulletActive = false;
		}

		//Vec3 vScale = Transform()->GetLocalScale();
		//Vec3 fDir = vBulletDir.Forward;
		//Vec3 fnDir = fDir.Normalize();
		//Transform()->SetLocalScale(Vec3(vScale.x + fnDir.x * 200, vScale.y + fnDir.y * 200, vScale.z + fnDir.z * 200));
	}
	else
	{
		Transform()->SetLocalPos(Vec3(-20000.f, -20000.f, -20000.f));
		
	}
}

void CBulletScript::OnCollisionEnter(CCollider2D* _pOther)
{
	if (BulletActive)
	{
		wstring s = _pOther->GetObj()->GetName();
		if (L"Monster Object" == _pOther->GetObj()->GetName())
		{
			//DeleteObject(GetObj());
			BulletActive = false;
		}

		if (L"Tomb Object" == _pOther->GetObj()->GetName())
		{
			//DeleteObject(GetObj());
			BulletActive = false;
		}
	}
}
