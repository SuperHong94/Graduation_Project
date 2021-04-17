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
	if (KEY_TAB(KEY_TYPE::KEY_SPACE))
	{
		sceneMgr->setaa(3);
		sceneMgr->SetIsChange(true);    
		sceneMgr->ChangeScene(sceneMgr->GetGameScene());
	}
}

