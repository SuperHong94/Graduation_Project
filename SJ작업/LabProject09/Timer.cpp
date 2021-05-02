#include "stdafx.h"
#include "Timer.h"


/*
CGameTimer 클래스의 생성자와 소멸자를 다음과 같이 정의한다. 컴퓨터가 성능 카운터(Performance Counter) 하드웨어를 가지고 있으면 성능 카운터와 성능
주파수(Performance Frequency)를 사용하여 시간 단위를 설정한다. 성능 주파수를 사용할
수 없으면 멀티미디어 타이머를 사용한다. 이 경우 시간 단위는 0.001초(Millisecond)이다. 성
능 카운터에 대한 자세한 설명은 윈도우 API 도움말을 참조하라.
*/

CGameTimer::CGameTimer()
{
	//퍼포먼스카원터의 주파수(시스템 부트될때 고정되어있고 모든프로세서를 걸쳐 한결같다.
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequency)) //퍼포먼스 카운터의 주파수를 검색한다.
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
		m_fTimeScale = 1.0f / m_nPerformanceFrequency;  //이부분 한번 확인해야겠다.
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = ::timeGetTime();
		m_fTimeScale = 0.001f;
	}
}
CGameTimer::~CGameTimer()
{

}

/*
 CGameTimer::Tick() 멤버 함수
컴퓨터(CPU)가 성능 카운터(Performance Counter) 하드웨어를 가지고 있으면 성능 카운터
와 성능 주파수(Performance Frequency)를 사용하여 현재 시간을 갱신하고 없으면 멀티미
디어 타이머를 사용한다.
*/
void CGameTimer::Tick(float fLockFPS)
{
	if (m_bHardwareHasPerformanceCounter) //성능 카운터가 있으면
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);  //현재시간에 
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}
	//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다.
	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale; //m_fTimeScaledms 1/m_nPerformanceFrequency로했었음(성능주파수있을경우)
	if (fLockFPS > 0.0f)
	{
		//이 함수의 파라메터(fLockFPS)가 0보다 크면 이 시간만큼 호출한 함수를 기다리게 한다.
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = ::timeGetTime();
			}
			//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다. 
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}
	//현재 시간을 m_nLastTime에 저장한다. 
	m_nLastTime = m_nCurrentTime;
	/* 마지막 프레임 처리 시간과 현재 프레임 처리 시간의 차이가 1초보다 작으면 현재 프레임 처리 시간
	을 m_fFrameTime[0]에 저장한다. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)  //fabs는 절대값
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float)); //memove(목적지, 소스, 49*size(float))
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT)
			m_nSampleCount++;
	}
	//초당 프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 저장한다.
	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond; //현재프레임레이트에 초당프레임수를 대입한다.
		m_nFramesPerSecond = 0;  //초당프레임수에 0 대입
		m_fFPSTimeElapsed = 0.0f; //프레임레이트 계산소요시간에 0대입
	}

	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구한다.
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++)
		m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0)
		m_fTimeElapsed /= m_nSampleCount;
}

/*
 CGameTimer::GetFrameRate() 멤버 함수
마지막 프레임 레이트를 문자열과 정수형으로 반환한다.
*/
unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 “ FPS”와 결합한다. 
	if (lpszString)
	{
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T("FPS)"));
	}
	return(m_nCurrentFrameRate);
}


float CGameTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}