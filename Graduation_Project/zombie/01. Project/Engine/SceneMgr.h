#pragma once
#include "GameMgr.h"

class CScene;
class CCamera;
class CGameObject;


class CSceneMgr
{
	SINGLE(CSceneMgr);
private:
	CScene* m_pStartScene;
	CScene* m_pEndScene;
	CScene* m_pGameScene;

	CScene* m_pCurScene;

	GameMgr* m_pGameManager;
	CGameObject* m_pPlayerArr[4];
	int			sceneNum = 0;
	bool		isChange = false;
public:
	int aa = 0;
	void setaa(int n) { aa = n; }
	void init();
	void initGameScene();
	void initStartScene();
	void initEndScene();

	void update();	 // Player Mod ¿œ∂ß
	void update_tool();

	CScene* GetStartScene() { return m_pStartScene; }
	CScene* GetGameScene() { return m_pGameScene; }
	CScene* GetEndScene() { return m_pEndScene; }

	bool CheckIsChange(){return isChange;}
	void SetIsChange(bool b) { isChange = b; }

public:
	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void FindGameObjectByTag( const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	void setMap();
	//void FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam = nullptr);


public:
	void CreateTargetUI();
};

