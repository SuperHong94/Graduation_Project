#pragma once

class CScene;
class CCamera;
class CGameObject;


class CSceneMgr
{
	SINGLE(CSceneMgr);
private:
	CScene*		m_pCurScene;	

public:
	void init();

	void update();	 // Player Mod ¿œ∂ß
	void update_tool(); 

public:
	CScene* GetCurScene();
	void ChangeScene(CScene* _pNextScene);
	void FindGameObjectByTag( const wstring& _strTag, vector<CGameObject*>& _vecFindObj);
	//void FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam = nullptr);
};

