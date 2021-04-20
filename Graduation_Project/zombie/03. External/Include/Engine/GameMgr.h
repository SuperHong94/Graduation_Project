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

	// ���� ������Ʈ ��������
	void tombArrInit(int i, CGameObject* tomb);

	// ���� �Ŵ��� ������Ʈ
	void GameMgrUpdate();

	// ���� ������ ��ġ ����
	void CheckZombieRespawn();

	// ����� ������ ��ġ ã��
	int FindNearRespawnPostion(Vec3 pos);

	// ���� �¸� ���� Ȯ��
	void CheckGameClear();

	// ���� �й� ���� Ȯ��
	void CheckGameOver();

	void GameInit();

	bool GetGameOver() { return Gstatus->isGameOver; };
	bool GetGameClear() { return Gstatus->isGameClear; };
};

