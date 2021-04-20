#pragma once
#include "Scene.h"
#include "GameObject.h"

struct GameMgrStatus
{
	int DeathZombieCnt = 0;
	int monsterCnt;
	float spawnPosition[4][2] = { 0, };

	CScene* Scene;
	CGameObject* monsterArr[100];
	CGameObject* playerArr[4];
	CGameObject* tombArr[4];

	int playerCnt;
	int playerID;

	bool isGameOver = false;
	bool isGameClear = false;

	int zombieGoalCnt = 20;
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

	// 게임 승리 조건 확인
	void CheckGameClear();

	// 게임 패배 조건 확인
	void CheckGameOver();

	void GameInit();

	bool GetGameOver() { return Gstatus->isGameOver; };
	bool GetGameClear() { return Gstatus->isGameClear; };
};

