#include "stdafx.h"
#include "BulletScript.h"


CBulletScript::CBulletScript(Vec3 Dir, float d)
	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
{
	this->SetName(L"BulletScript");

	vBulletDir = Dir;
	setDamage(d);
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
	vPos += vBulletDir * 1000 * DT;
	Transform()->SetLocalPos(vPos);

	if (vPos.x >= 5000 || vPos.x <= -5000 || vPos.y >= 5000 || vPos.y <= -5000)
		DeleteObject(GetObj());
}

void CBulletScript::OnCollisionEnter(CCollider2D* _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}
}
