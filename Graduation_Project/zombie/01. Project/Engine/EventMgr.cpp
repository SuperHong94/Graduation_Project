#include "stdafx.h"
#include "EventMgr.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"
#include "Component.h"

CEventMgr::CEventMgr()
{
}

CEventMgr::~CEventMgr()
{
}

void CEventMgr::update()
{
	// 삭제 예정 Objects 메모리 해제
	for (size_t i = 0; i < m_vecDead.size(); ++i)
	{		
		m_vecDead[i]->ClearParent();
		SAFE_DELETE(m_vecDead[i]);
	}
	m_vecDead.clear();

	// Event 처리
	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		Execute(m_vecEvent[i]);
	}
}

void CEventMgr::Execute(tEvent & _event)
{
	switch (_event.eType)
	{
	case EVENT_TYPE::CREATE_OBJECT:		
		CSceneMgr::GetInst()->GetCurScene()->AddGameObject((int)_event.lParam, (CGameObject*)_event.wParam, true);
		break;

	case EVENT_TYPE::DELETE_OBJECT:
		if (((CGameObject*)_event.wParam)->m_bDead)
			break;

		((CGameObject*)_event.wParam)->SetDead();
		m_vecDead.push_back((CGameObject*)_event.wParam);

		break;
	case EVENT_TYPE::ADD_CHILD:
		((CGameObject*)_event.wParam)->AddChild((CGameObject*)_event.lParam);
		break;

	case EVENT_TYPE::CLEAR_PARENT:
		((CGameObject*)_event.wParam)->ClearParent();
		break;

	case EVENT_TYPE::TRANSFER_LAYER:		
	{
		bool bMoveAll = LOWORD(_event.lParam);
		int iLayerIdx = HIWORD(_event.lParam);		
		CSceneMgr::GetInst()->GetCurScene()->AddGameObject(iLayerIdx, ((CGameObject*)_event.wParam), bMoveAll);		
	}
		break;

	case EVENT_TYPE::ACTIVATE_GAMEOBJECT:
		((CGameObject*)_event.wParam)->enable();
		((CGameObject*)_event.wParam)->m_bActive = true;
		break;

	case EVENT_TYPE::DEACTIVATE_GAMEOBJECT:
		if (((CGameObject*)_event.wParam)->IsDead())
			break;

		((CGameObject*)_event.wParam)->disable();
		((CGameObject*)_event.wParam)->m_bActive = false;
		break;

	case EVENT_TYPE::ACTIVATE_COMPONENT:
		((CComponent*)_event.wParam)->enable();
		((CComponent*)_event.wParam)->m_bActive = true;
		break;

	case EVENT_TYPE::DEACTIVATE_COMPONENT:
		((CComponent*)_event.wParam)->disable();
		((CComponent*)_event.wParam)->m_bActive = false;
		break;

	case EVENT_TYPE::TRANSFER_SCENE:

		break;
	}
}