#pragma once
#include "Scene.h"
#include "GameObject.h"

class GameMgr
{
private:
	int DeathZombieCnt = 0;
	int monsterCnt;

	CScene* Scene;
	CGameObject* monsterArr[100];
public:
	GameMgr(CScene* pScene, CGameObject* monsters[], int monsterCount);
	~GameMgr();

	// ���� ������ ��ġ ����
	void CheckZombieRespawn();
};

