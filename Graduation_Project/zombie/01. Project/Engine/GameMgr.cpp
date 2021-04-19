#include "stdafx.h"
#include "GameMgr.h"
#include "GameObject.h"
#include "Script.h"

#include "Transform.h"

#include "MonsterScript.h"


GameMgr::GameMgr(CScene* pScene, CGameObject* monsters[], int monsterCount, CGameObject* PlayerArr[], int PlayerCnt, int PlayerID)
{
	Gstatus->Scene = new CScene;
	Gstatus->Scene = pScene;
	Gstatus->monsterCnt = monsterCount;

	for (int i = 0; i < Gstatus->monsterCnt; i++)
	{
		Gstatus->monsterArr[i] = monsters[i];
	}

	Gstatus->playerCnt = PlayerCnt;

	for (int i = 0; i < Gstatus->playerCnt; i++)
	{
		Gstatus->playerArr[i] = PlayerArr[i];
	}

	Gstatus->playerID = PlayerID;
}

GameMgr::~GameMgr()
{
}

void GameMgr::CheckZombieRespawn()
{
	for (int i = 0; i < Gstatus->monsterCnt; i++)
	{
		if (Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->GetStatus()->IsDisappear)
		{
			Vec3 vPos = Gstatus->monsterArr[i]->Transform()->GetLocalPos();
			int respawnNum = FindNearRespawnPostion(vPos);

			Gstatus->monsterArr[i]->Transform()->SetLocalPos(Vec3(Gstatus->spawnPosition[respawnNum][0], 0, Gstatus->spawnPosition[respawnNum][1]));

			MonsterStatus* status = Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->GetStatus();

			status->state = MonsterState::M_Respawn;
			status->distanceToPlayer = 0;
			status->attackRange = 100;
			status->PlayerInRange = false;
			status->PlayerInAttackRange = false;
			status->isAttack = false;
			status->hp = 100;
			status->disappearCnt = 0;
			status->IsDisappear = false;

			Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->SetStatus(status);

			// 멀티 시에는 서버가 계산할 것
			// 임시로 여기서 함
			Gstatus->DeathZombieCnt++;
		}
	}
}


void GameMgr::GameMgrUpdate()
{
	CheckGameOver();
	if (Gstatus->isGameOver)
	{
		//씬 체인지
		return;
	}

	CheckGameClear();
	if (Gstatus->isGameOver)
	{
		//씬 체인지
		return;
	}

	CheckZombieRespawn();
}

void GameMgr::CheckGameClear()
{
	if (Gstatus->DeathZombieCnt >= 3)
	{
		Gstatus->isGameClear = true;
	}
}

void GameMgr::CheckGameOver()
{
	int cnt = 0;
	for (int i = 0; i < Gstatus->playerCnt; i++)
	{
		if (Gstatus->playerArr[i]->GetScript<CPlayerScript>()->GetStatus()->isDisappear)
			cnt++;
	}

	if (cnt == Gstatus->playerCnt && !Gstatus->isGameOver)
		Gstatus->isGameOver = true;
}


int GameMgr::FindNearRespawnPostion(Vec3 pos)
{
	int n = 0;
	float distance = 99999999;

	for (int i = 0; i < 4; i++)
	{
		float temp = sqrt((pos.x - Gstatus->spawnPosition[i][0]) * (pos.x - Gstatus->spawnPosition[i][0]) + (pos.z - Gstatus->spawnPosition[i][1]) * (pos.z - Gstatus->spawnPosition[i][1]));
		if (temp < distance)
		{
			distance = temp;
			n = i;
		}
	}

	return n;
}

