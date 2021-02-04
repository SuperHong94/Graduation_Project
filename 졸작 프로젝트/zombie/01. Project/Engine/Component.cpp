#include "stdafx.h"
#include "Component.h"

#include "EventMgr.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	: m_eComType(_eType)
	, m_bChange(true)
	, m_bActive(true)
{
}

CComponent::~CComponent()
{
}

void CComponent::SetActive(bool _bTrue)
{
	if (m_bActive)
	{
		if (!_bTrue)
		{
			// 비활성화
			tEvent event = {};
			event.eType = EVENT_TYPE::DEACTIVATE_COMPONENT;
			event.wParam = (DWORD_PTR)this;

			CEventMgr::GetInst()->AddEvent(event);
		}
	}
	else
	{
		if (_bTrue)
		{
			// 활성화
			// 비활성화
			tEvent event = {};
			event.eType = EVENT_TYPE::ACTIVATE_COMPONENT;
			event.wParam = (DWORD_PTR)this;

			CEventMgr::GetInst()->AddEvent(event);
		}
	}
}
