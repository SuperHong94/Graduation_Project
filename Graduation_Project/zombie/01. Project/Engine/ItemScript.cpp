#include "stdafx.h"
#include "ItemScript.h"
#include "PlayerScript.h"

CItemScript::CItemScript(ItemState State)
	: CScript((UINT)SCRIPT_TYPE::ITEMSCRIPT)
{
	state = State;
	activeTime = 0.f;
	isSpawn = false;
}

CItemScript::~CItemScript()
{
}


void CItemScript::update()
{
	if (isSpawn)
	{
		int a = 3;
		if (activeTime > 0)
		{
			activeTime -= DT;

			if (state == ItemState::I_HpItem)
			{
				Vec3 vRot = Transform()->GetLocalRot();
				Transform()->SetLocalRot(Vec3(vRot.x, vRot.y + DT, 0.f));
			}
			
			else
			{
				float rot = Transform()->GetLocalRot().y;
				Transform()->SetLocalRot(Vec3(0.f, rot + DT, 0.f));
			}
		}

		else
		{
			activeTime = 0;
			isSpawn = false;
			Transform()->SetLocalPos(Vec3(20000.f, 0.f, 20000.f));
		}
	}
}

void CItemScript::OnCollisionEnter(CCollider2D* _pOther)
{
	if (isSpawn)
	{
		int a = 3;
		wstring s = _pOther->GetObj()->GetName();
		if (L"Player Object" == _pOther->GetObj()->GetName())
		{
			if (!_pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->IsDead)
			{
				if (state == ItemState::I_PwPotion)
					_pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->powerBuffTime += 10.f;

				else if (state == ItemState::I_DfPotion)
					_pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->defenceBuffTime += 10.f;

				else if (state == ItemState::I_SpPotion)
					_pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->speedBuffTime += 10.f;

				else if (state == ItemState::I_HpItem)
				{
					float hp = _pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->hp;
					hp += 20;
					if (hp > 100)
						hp = 100;
					_pOther->GetObj()->GetScript<CPlayerScript>()->GetStatus()->hp = hp;
				}

				Transform()->SetLocalPos(Vec3(20000.f, 0.f, 20000.f));
				activeTime = 0.f;
				isSpawn = false;
			}
		}
	}
}