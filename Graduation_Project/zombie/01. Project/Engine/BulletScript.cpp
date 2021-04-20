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

	Vec3 vPos = Transform()->GetLocalPos();
	//vPos.y += 400.f * DT;
	vPos += vBulletDir * BulletSpeed * DT;
	Transform()->SetLocalPos(vPos);

	if (vPos.x >= 6000 || vPos.x <= -6000 || vPos.z >= 6000 || vPos.z <= -6000)
	{
		DeleteObject(GetObj());
	}
}

void CBulletScript::OnCollisionEnter(CCollider2D* _pOther)
{
	wstring s = _pOther->GetObj()->GetName();
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}

	if (L"Tomb Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}
}
