#include "stdafx.h"
#include "StartSceneScript.h"

CStartSceneScript::CStartSceneScript(CSceneMgr* SceneMgr)
	: CScript((UINT)SCRIPT_TYPE::STARTSCENESCRIPT)
{
	sceneMgr = SceneMgr;
}

CStartSceneScript::~CStartSceneScript()
{
}

void CStartSceneScript::update()
{
}

void CStartSceneScript::finalupdate()
{
}

