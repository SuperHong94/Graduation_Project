#include "stdafx.h"
#include "TimeMgr.h"

#include "Core.h"

CTimeMgr::CTimeMgr()
	: m_llCurCount{}
	, m_llOldCount{}
	, m_llFrequency{}
	, m_fDeltaTime (0.f)
	, m_fAccTime(0.f)
	, m_bFreeze(false)
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::init()
{
	QueryPerformanceFrequency(&m_llFrequency);
	QueryPerformanceCounter(&m_llOldCount);
}

void CTimeMgr::update()
{
	QueryPerformanceCounter(&m_llCurCount);
	
	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llOldCount.QuadPart) / (float)m_llFrequency.QuadPart;
	m_llOldCount = m_llCurCount;

	m_fAccTime += m_fDeltaTime;	

	if (m_bFreeze)
	{
		m_bFreeze = false;
		m_fDeltaTime = 1.f / 60.f;
	}

	if (m_fAccTime > 1.f)
	{
		QueryPerformanceFrequency(&m_llFrequency);
		m_fAccTime = 0.f;
		m_fFPS = 1 / m_fDeltaTime;
	}

	wchar_t szFPS[50] = L"";
	wsprintf(szFPS, L"FPS : %d", (int)m_fFPS);

	SetWindowText(CCore::GetInst()->m_hMainHwnd, szFPS);
}
//
//void CTimeMgr::render(HDC _dc)
//{	
//	if (m_fAccTime > 1.f)
//	{
//		QueryPerformanceFrequency(&m_llFrequency);
//		m_fAccTime = 0.f;
//		m_fFPS = 1 / m_fDeltaTime;
//	}
//}