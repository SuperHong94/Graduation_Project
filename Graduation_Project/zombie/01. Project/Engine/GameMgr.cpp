#include "stdafx.h"
#include "GameMgr.h"
#include "GameObject.h"
#include "Script.h"

#include "Transform.h"

#include "MonsterScript.h"


GameMgr::GameMgr(CScene* pScene, CGameObject* monsters[], int monsterCount)
{
	Scene = pScene;
	monsterCnt = monsterCount;

	for (int i = 0; i < monsterCnt; i++)
	{
		monsterArr[i] = monsters[i];
	}
}

GameMgr::~GameMgr()
{
}

void GameMgr::CheckZombieRespawn()
{
	for (int i = 0; i < monsterCnt; i++)
	{
		if (monsterArr[i]->GetScript<CMonsterScript>()->GetStatus()->IsDisappear)
		{
			monsterArr[i]->Transform()->SetLocalPos(Vec3(0, 0, 0));

			MonsterStatus* status = monsterArr[i]->GetScript<CMonsterScript>()->GetStatus();

			status->state = MonsterState::M_Respawn;
			status->distanceToPlayer = 0;
			status->attackRange = 100;
			status->PlayerInRange = false;
			status->PlayerInAttackRange = false;
			status->isAttack = false;
			status->hp = 100;
			status->disappearCnt = 0;
			status->IsDisappear = false;

			monsterArr[i]->GetScript<CMonsterScript>()->SetStatus(status);
		}
	}
}

