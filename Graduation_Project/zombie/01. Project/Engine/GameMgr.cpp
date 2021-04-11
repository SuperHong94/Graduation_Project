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
			Vec3 vPos = monsterArr[i]->Transform()->GetLocalPos();
			int respawnNum = FindNearRespawnPostion(vPos);

			monsterArr[i]->Transform()->SetLocalPos(Vec3(spawnPosition[respawnNum][0], 0, spawnPosition[respawnNum][1]));

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

int GameMgr::FindNearRespawnPostion(Vec3 pos)
{
	int n = 0;
	float distance = 99999999;

	for (int i = 0; i < 4; i++)
	{
		float temp = sqrt((pos.x - spawnPosition[i][0]) * (pos.x - spawnPosition[i][0]) + (pos.z - spawnPosition[i][1]) * (pos.z - spawnPosition[i][1]));
		if (temp < distance)
		{
			distance = temp;
			n = i;
		}
	}

	return n;
}

