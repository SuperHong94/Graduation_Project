#include "stdafx.h"
#include "Scene.h"

#include "Layer.h"
#include "GameObject.h"
#include "Camera.h"

#include "EventMgr.h"

CScene::CScene()
	: m_arrLayer{}
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		AddLayer(L"", i);
	}	
}

CScene::~CScene()
{
	Safe_Delete_Array(m_arrLayer);
}

void CScene::awake()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr != m_arrLayer[i])
			m_arrLayer[i]->awake();
	}
}

void CScene::start()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr != m_arrLayer[i])
			m_arrLayer[i]->start();
	}

	
}

void CScene::update()
{
	for (UINT i = 0; i < MAX_LAYER ; ++i)
	{
		if(nullptr != m_arrLayer[i])
			m_arrLayer[i]->update();
	}
}

void CScene::lateupdate()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr != m_arrLayer[i])
			m_arrLayer[i]->lateupdate();
	}
}

void CScene::finalupdate()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr != m_arrLayer[i])
			m_arrLayer[i]->finalupdate();
	}
}


void CScene::AddLayer(const wstring & _strLayerName, int _iIdx)
{	
	// 비정상적인 LayerIndex 거르기
	assert(-1 < _iIdx && _iIdx <= 31 && (nullptr == m_arrLayer[_iIdx]));

	m_arrLayer[_iIdx] = new CLayer;
	m_arrLayer[_iIdx]->SetName(_strLayerName);
	m_arrLayer[_iIdx]->SetLayerIdx(_iIdx);
}

void CScene::SetLayerName(int _iIdx, const wstring & _strName)
{
	assert(-1 < _iIdx && _iIdx <= 31);
	m_arrLayer[_iIdx]->SetName(_strName);
}

void CScene::AddGameObject(const wstring & _strLayerName, CGameObject * _pObject, bool _bMoveAll)
{
	CLayer* pLayer = FindLayer(_strLayerName);
	assert(pLayer);	
	pLayer->AddGameObject(_pObject, _bMoveAll);
}

void CScene::AddGameObject(int _iLayerIdx, CGameObject * _pObject, bool _bMoveAll)
{
	assert(-1 < _iLayerIdx && _iLayerIdx <= 31);
	CLayer* pLayer = GetLayer(_iLayerIdx);
	pLayer->AddGameObject(_pObject, _bMoveAll);
}

CLayer * CScene::FindLayer(const wstring & _strLayerName)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		if (_strLayerName == m_arrLayer[i]->GetName())
			return m_arrLayer[i];
	}

	return nullptr;
}
