#include "stdafx.h"
#include "BulletScript.h"


CBulletScript::CBulletScript(Vec3 Dir)
	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
{
	vBulletDir = Dir;
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
	vPos += vBulletDir * 400 * DT;
	Transform()->SetLocalPos(vPos);
}

void CBulletScript::OnCollisionEnter(CCollider2D * _pOther)
{
	if (L"Monster Object" == _pOther->GetObj()->GetName())
	{
		DeleteObject(GetObj());
	}
}
