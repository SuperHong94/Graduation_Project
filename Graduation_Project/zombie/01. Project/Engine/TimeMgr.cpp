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

	static int i = 0;
	++i;
	if (m_fAccTime >= 1.f)
	{
		QueryPerformanceFrequency(&m_llFrequency);

		m_fFPS = 1 / m_fDeltaTime;

		// FPS 출력
		wchar_t szFPS[50] = L"";
		wsprintf(szFPS, L"FPS : %d", (int)i/ (int)m_fAccTime);
		//wsprintf(szFPS, L"FPS : %d", i / 2);
		SetWindowText(CCore::GetInst()->m_hMainHwnd, szFPS);
		i = 0;
		m_fAccTime = 0.f;
	}

	// 전역 상수버퍼에 전달 될 값
	g_global.fDT = m_fDeltaTime;
	g_global.fAccTime += m_fDeltaTime;


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