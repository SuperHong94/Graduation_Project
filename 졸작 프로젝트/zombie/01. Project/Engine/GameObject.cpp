#include "stdafx.h"
#include "GameObject.h"

#include "Component.h"
#include "MeshRender.h"

CGameObject::CGameObject()
	: m_arrCom{}
{
}

CGameObject::~CGameObject()
{
	Safe_Delete_Array(m_arrCom);
}

void CGameObject::AddComponent(CComponent * _pCom)
{
	m_arrCom[(UINT)_pCom->GetComType()] = _pCom;
	_pCom->SetGameObject(this);
}

void CGameObject::awake()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		m_arrCom[i]->awake();
	}
}

void CGameObject::start()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		m_arrCom[i]->start();
	}
}

void CGameObject::update()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		m_arrCom[i]->update();
	}
}

void CGameObject::lateupdate()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		m_arrCom[i]->lateupdate();
	}
}

void CGameObject::finalupdate()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		m_arrCom[i]->finalupdate();
	}
}

void CGameObject::render()
{
	if (nullptr == MeshRender())
		return;

	MeshRender()->render();
}
