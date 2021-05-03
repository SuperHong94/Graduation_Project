#pragma once
#include "GameMgr.h"

class CScene;
class CCamera;
class CGameObject;

//enum class SCENE_STATE 
//{
//	START_SCENE,
//	GAME_SCENE,
//	GAMECLEAR_SCENE,
//	GAMEOVER_SCENE,
//};

class CSceneMgr
{
	SINGLE(CSceneMgr);
private:
	CScene* m_pCurScene;

	GameMgr* m_pGameManager;
	CGameObject* m_pPlayerArr[4];
	int			sceneNum = 0;
	bool		isChange = false;

	// 서버와 통신 해야됨
	int playerNum = 4;
	// 임시 설정 
	int playerID = 0;

	float posUIRatio = 64;
	float miniMapUIRatio = 6.8;

	SCENE_STATE SceneState = SCENE_STATE::START_SCENE;
public:
	SCENE_STATE setSceneNum(SCENE_STATE s) { SceneState = s; }
	void init();
	void initGameScene();
	void initStartScene();
	void initEndScene();

	void update();	 // Player Mod 일때
	void update_tool();

	void updateUI();
	/*CScene* GetStartScene() { return m_pStartScene; }
	CScene* GetGameScene() { return m_pGameScene; }
	CScene* GetEndScene() { return m_pEndScene; }*/

	bool CheckIsChange(){return isChange;}
	void SetIsChange(bool b) { isChange = b; }

public:
<<<<<<< HEAD
	void NewCurScene() { m_pCurScene = new CScene; };
=======
>>>>>>> yjs
	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void FindGameObjectByTag( const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	void setMap();
	//void FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam = nullptr);


public:
	void CreateTargetUI();
};

