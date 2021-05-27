#pragma once
#include "GameMgr.h"

class CScene;
class CCamera;
class CGameObject;

enum class SCENE_STATE 
{
	START_SCENE,
	GAME_SCENE,
	GAMECLEAR_SCENE,
	GAMEOVER_SCENE,
};

class CSceneMgr
{
	SINGLE(CSceneMgr);
private:
	CScene* m_pCurScene;
	GameMgr* m_pGameManager;
	CGameObject* m_pPlayerArr[4];
	CGameObject* monsterArr[100];
	CGameObject* m_pItem[40];
	int monsterCnt = 60;
	int ItemCnt = 50;
	int			sceneNum = 0;
	bool		isChange = false;
	float collOffset = 20000.f;
	bool QuestVisible = true;
	bool pressI = false;

	// ������ ��� �ؾߵ�
	int playerNum = 4;
	// �ӽ� ���� 
	int playerID = 0;

	float posUIRatio = 64;
	float miniMapUIRatio = 6.8;
	float hpBarWidth = 0;
	float hpBarHeight = 0;

	SCENE_STATE SceneState = SCENE_STATE::START_SCENE;
public:
	SCENE_STATE setSceneNum(SCENE_STATE s) { SceneState = s; }
	void init();
	void initGameScene();
	void initStartScene();
	void initEndScene();

	void update();	 // Player Mod �϶�
	void update_tool();

	void updateUI();

	/*CScene* GetStartScene() { return m_pStartScene; }
	CScene* GetGameScene() { return m_pGameScene; }
	CScene* GetEndScene() { return m_pEndScene; }*/

	bool CheckIsChange(){return isChange;}
	void SetIsChange(bool b) { isChange = b; }

	float GetCollOffset() { return collOffset; };
	void SetCollOffset(float offset) { collOffset = offset; };

	//�ʱ�ȭ�� ��
	void initValue();

public:
	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void FindGameObjectByTag( const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	void setMap();
	//void FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam = nullptr);


public:
	void CreateTargetUI();
};

