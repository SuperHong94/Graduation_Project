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
	Vec3 vPos = Transform()->GetLocalPos();
	//vPos.y += 400.f * DT;
	vPos += vBulletDir * BulletSpeed * DT;
	Transform()->SetLocalPos(vPos);

	if (vPos.x >= 5000 || vPos.x <= -5000 || vPos.z >= 5000 || vPos.z <= -5000)
	{
		DeleteObject(GetObj());
	}
}

void CBulletScript::OnCollisionEnter(CCollider2D* _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}
}
