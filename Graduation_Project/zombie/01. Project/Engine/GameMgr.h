#pragma once
#include "Scene.h"
#include "GameObject.h"

struct GameMgrStatus
{
	int DeathZombieCnt = 0;
	int monsterCnt;
	float spawnPosition[4][2] = { 0, };
	float destroySpawnPosition[4][2] = { {5050.f, 0.f}, {-5050.f, 0.f}, {0.f, 5050.f}, {0.f, -5050.f} };

	CScene* Scene;
	CGameObject* monsterArr[100];
	CGameObject* playerArr[4];
	CGameObject* tombArr[4];

	int playerCnt = 4;
	int playerID;

	bool isGameOver = false;
	bool isGameClear = false;
	bool bossDead = false;
	bool bossSpawn = false;

	int zombieGoalCnt = 10;

	int destroyTombCnt = 0;
	bool tombAllDestroy = false;

	float addZombieRange = 0;

	bool cheatBossSpawn = false;
	bool cheatDestroyTombOn = false;

	float TotalTime = 300.f;
	float Timer = 300.f;
};

class GameMgr
{
private:
	GameMgrStatus* Gstatus = new GameMgrStatus;

public:
	GameMgr(CScene* pScene, CGameObject* monsters[], int monsterCount, CGameObject* PlayerArr[], int PlayerCnt, int PlayerID);
	~GameMgr();


	// 무덤 오브젝트 가져오기
	void tombArrInit(int i, CGameObject* tomb);

	// 게임 매니저 업데이트
	void GameMgrUpdate();

	// 좀비 리스폰 위치 관리
	void CheckZombieRespawn();

	// 가까운 리스폰 위치 찾기
	int FindNearRespawnPostion(Vec3 pos);

	// 보스 스폰 조건 확인
	void ChekBossSpawn();

	// 게임 승리 조건 확인
	void CheckGameClear();

	// 게임 패배 조건 확인
	void CheckGameOver();

	// 좀비 인식 범위 증가
	void IncreaseZombieRange();

	// 보스 죽음 확인
	void checkBossDead();

	void GameInit();

	// 치트 무덤 파괴
	void cheatDestroyTomb();

	// 치트 보스 스폰
	void cheatBossSpawn() { Gstatus->cheatBossSpawn = true; };

	// 타이머 업데이트
	void TimerUpdate();

	bool GetGameOver() { return Gstatus->isGameOver; };
	bool GetGameClear() { return Gstatus->isGameClear; };

	float GetTimer() { return Gstatus->Timer; };
	float GetTotalTimer() { return Gstatus->TotalTime; };
};

