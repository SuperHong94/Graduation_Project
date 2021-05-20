#pragma once
#include "Script.h"
#include "Scene.h"
#include "SceneMgr.h"

class CStartSceneScript :
	public CScript
{
private:
	CSceneMgr* sceneMgr;

public:
	virtual void update();
	virtual void finalupdate();

public:
	CLONE(CStartSceneScript);

public:
	CStartSceneScript(CSceneMgr* SceneMgr);
	virtual ~CStartSceneScript();
};

