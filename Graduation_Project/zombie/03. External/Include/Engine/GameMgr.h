#pragma once
#include "Scene.h"
#include "GameObject.h"

struct GameMgrStatus
{
	int DeathZombieCnt = 0;
	int monsterCnt;
	float spawnPosition[4][2] = { {0.f, 5000.f}, {0.f, -5000.f}, {-5000.f, 0.f}, {5000.f, 0.f} };

	CScene* Scene;
	CGameObject* monsterArr[100];
	CGameObject* playerArr[4];
	int playerCnt;
	int playerID;

	bool isGameOver = false;
	bool isGameClear = false;
};

class GameMgr
{
private:
	GameMgrStatus* Gstatus = new GameMgrStatus;

public:
	GameMgr(CScene* pScene, CGameObject* monsters[], int monsterCount, CGameObject* PlayerArr[], int PlayerCnt, int PlayerID);
	~GameMgr();

	// ���� �Ŵ��� ������Ʈ
	void GameMgrUpdate();

	// ���� ������ ��ġ ����
	void CheckZombieRespawn();

	// ����� ������ ��ġ ã��
	int FindNearRespawnPostion(Vec3 pos);

	// ���� �¸� ���� Ȯ��
	void IsGameClear();

	// ���� �й� ���� Ȯ��
	void IsGameOver();

	void GameInit();
};

