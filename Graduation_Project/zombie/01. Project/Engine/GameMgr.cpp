#include "stdafx.h"
#include "GameMgr.h"
#include "GameObject.h"
#include "Script.h"

#include "Transform.h"

#include "MonsterScript.h"
#include "TombScript.h"

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
		// 죽은 좀비
		if (Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->GetStatus()->IsDisappear)
		{
			Vec3 vPos = Gstatus->monsterArr[i]->Transform()->GetLocalPos();
			int respawnNum = FindNearRespawnPostion(vPos);

			// 무덤이 전부 파괴 x 일시
			if (!Gstatus->tombAllDestroy)
				Gstatus->monsterArr[i]->Transform()->SetLocalPos(Vec3(Gstatus->spawnPosition[respawnNum][0], 0, Gstatus->spawnPosition[respawnNum][1]));

			// 무덤이 전부 파괴 o 일시
			else
				Gstatus->monsterArr[i]->Transform()->SetLocalPos(Vec3(Gstatus->destroySpawnPosition[respawnNum][0], 0, Gstatus->destroySpawnPosition[respawnNum][1]));

			MonsterStatus* status = Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->GetStatus();

			status->state = MonsterState::M_Respawn;
			status->distanceToPlayer = 0;
			status->attackRange = 100;
			status->PlayerInRange = false;
			status->PlayerInAttackRange = false;
			status->isAttack = false;
			status->recognizeRange = 1000.f + Gstatus->addZombieRange;
			status->hp = 100;
			status->disappearCnt = 0;
			status->IsDisappear = false;

			Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->SetStatus(status);

			// 멀티 시에는 서버가 계산할 것
			// 임시로 여기서 함
			Gstatus->DeathZombieCnt++;
		}

		// 죽지 않은 좀비
		else
		{
			MonsterStatus* status = Gstatus->monsterArr[i]->GetScript<CMonsterScript>()->GetStatus();

			status->recognizeRange = 1000.f + Gstatus->addZombieRange;
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

	// 무덤이 전부 파괴 안될 시에만 실행
	if(!Gstatus->tombAllDestroy)
		IncreaseZombieRange();

	CheckZombieRespawn();
}

void GameMgr::CheckGameClear()
{
	if (Gstatus->DeathZombieCnt >= Gstatus->zombieGoalCnt)
	{
		Gstatus->isGameClear = true;
		Gstatus->isGameOver = false;
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
	{
		Gstatus->isGameOver = true;
		Gstatus->isGameClear = false;
	}
}

void GameMgr::IncreaseZombieRange()
{
	int TombCnt = 0;

	for (int i = 0; i < 4; i++)
	{
		if (Gstatus->tombArr[i]->GetScript<CTombScript>()->CheckIsDisappear())
		{
			TombCnt++;
		}
	}

	// 무덤 파괴될 때마다 좀비 인지 범위 상승
	if (TombCnt > Gstatus->destroyTombCnt)
	{
		int increaseRange = TombCnt - Gstatus->destroyTombCnt;

		Gstatus->addZombieRange += increaseRange * 1500.f;

		Gstatus->destroyTombCnt = TombCnt;
	}

	// 무덤이 전부 파괴 되었는지 판단
	if (Gstatus->destroyTombCnt == 4)
		Gstatus->tombAllDestroy = true;
}


void GameMgr::tombArrInit(int i, CGameObject* tomb)
{
	Gstatus->tombArr[i] = tomb;

	Gstatus->spawnPosition[i][0] = Gstatus->tombArr[i]->Transform()->GetLocalPos().x;
	Gstatus->spawnPosition[i][1] = Gstatus->tombArr[i]->Transform()->GetLocalPos().z;
}

int GameMgr::FindNearRespawnPostion(Vec3 pos)
{
	int n = 0;
	float distance = 99999999;

	for (int i = 0; i < 4; i++)
	{
		if (!Gstatus->tombAllDestroy)
		{
			if (!Gstatus->tombArr[i]->GetScript<CTombScript>()->CheckIsDisappear())
			{
				float temp = sqrt((pos.x - Gstatus->spawnPosition[i][0]) * (pos.x - Gstatus->spawnPosition[i][0]) + (pos.z - Gstatus->spawnPosition[i][1]) * (pos.z - Gstatus->spawnPosition[i][1]));
				if (temp < distance)
				{
					distance = temp;
					n = i;
				}
			}
		}

		else
		{
			float temp = sqrt((pos.x - Gstatus->destroySpawnPosition[i][0]) * (pos.x - Gstatus->destroySpawnPosition[i][0]) + (pos.z - Gstatus->destroySpawnPosition[i][1]) * (pos.z - Gstatus->destroySpawnPosition[i][1]));
			if (temp < distance)
			{
				distance = temp;
				n = i;
			}
		}
	}


	return n;
}

