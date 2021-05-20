#include "stdafx.h"
#include "Script.h"

CScript::CScript(UINT _iScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_iScriptType(_iScriptType)
{
}

CScript::~CScript()
{
}

void CScript::CreateObject(CGameObject * _pNewObject, int _iLayerIdx)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::CREATE_OBJECT;
	evt.wParam = (DWORD_PTR)_pNewObject;
	evt.lParam = (DWORD_PTR)_iLayerIdx;

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::CreateObject(CGameObject * _pNewObject, const wstring & _strLayerName)
{
	// 생성 이벤트 등록
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(_strLayerName);
	if (nullptr == pLayer)
		assert(nullptr);

	tEvent evt = {};

	evt.eType = EVENT_TYPE::CREATE_OBJECT;
	evt.wParam = (DWORD_PTR)_pNewObject;
	evt.lParam = (DWORD_PTR)pLayer->GetLayerIdx();

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::DeleteObject(CGameObject * _pDeleteObject)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::DELETE_OBJECT;
	evt.wParam = (DWORD_PTR)_pDeleteObject;	

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::AddChild(CGameObject * _pChildObject)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::ADD_CHILD;
	evt.wParam = (DWORD_PTR)GetObj();
	evt.lParam = (DWORD_PTR)_pChildObject;

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::AddChild(CGameObject * _pParent, CGameObject * _pChild)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::ADD_CHILD;
	evt.wParam = (DWORD_PTR)_pParent;
	evt.lParam = (DWORD_PTR)_pChild;

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::ClearParent()
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::CLEAR_PARENT;
	evt.wParam = (DWORD_PTR)GetObj();	

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::ClearParent(CGameObject * _pTarget)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::CLEAR_PARENT;
	evt.wParam = (DWORD_PTR)_pTarget;	

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::TransferLayer(const wstring & _strLayerName, bool _bMoveAll)
{
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(_strLayerName);
	assert(nullptr != pLayer);

	if (GetObj()->GetLayerIdx() == pLayer->GetLayerIdx())
		return;

	tEvent evt = {};

	evt.eType = EVENT_TYPE::TRANSFER_LAYER;
	evt.wParam = (DWORD_PTR)GetObj();
	evt.lParam = ((DWORD_PTR)pLayer->GetLayerIdx() << 16 | (DWORD_PTR)_bMoveAll);

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::TransferLayer(int _iLayerIdx, bool _bMoveAll)
{
	assert(0 <= _iLayerIdx && _iLayerIdx < 32);

	tEvent evt = {};

	evt.eType = EVENT_TYPE::TRANSFER_LAYER;
	evt.wParam = (DWORD_PTR)GetObj();
	evt.lParam = ((DWORD_PTR)_iLayerIdx << 16 | (DWORD_PTR)_bMoveAll);

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::TransferLayer(CGameObject * _pTarget, const wstring & _strLayerName, bool _bMoveAll)
{
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(_strLayerName);
	assert(nullptr != pLayer);

	if (_pTarget->GetLayerIdx() == pLayer->GetLayerIdx())
		return;

	tEvent evt = {};

	evt.eType = EVENT_TYPE::TRANSFER_LAYER;
	evt.wParam = (DWORD_PTR)_pTarget;
	evt.lParam = ((DWORD_PTR)pLayer->GetLayerIdx() << 16 | (DWORD_PTR)_bMoveAll);

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::TransferLayer(CGameObject * _pTarget, int _iLayerIdx, bool _bMoveAll)
{
	assert(0 <= _iLayerIdx && _iLayerIdx < 32);

	tEvent evt = {};

	evt.eType = EVENT_TYPE::TRANSFER_LAYER;
	evt.wParam = (DWORD_PTR)_pTarget;
	evt.lParam = ((DWORD_PTR)_iLayerIdx << 16 | (DWORD_PTR)_bMoveAll);

	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::EableObject(CGameObject * _pTarget)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::ACTIVATE_GAMEOBJECT;
	evt.wParam = (DWORD_PTR)_pTarget;
	CEventMgr::GetInst()->AddEvent(evt);
}

void CScript::Disable(CGameObject * _pTarget)
{
	tEvent evt = {};

	evt.eType = EVENT_TYPE::DEACTIVATE_GAMEOBJECT;
	evt.wParam = (DWORD_PTR)_pTarget;
	CEventMgr::GetInst()->AddEvent(evt);
}
