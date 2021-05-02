#include "stdafx.h"
#include "Timer.h"


/*
CGameTimer Ŭ������ �����ڿ� �Ҹ��ڸ� ������ ���� �����Ѵ�. ��ǻ�Ͱ� ���� ī����(Performance Counter) �ϵ��� ������ ������ ���� ī���Ϳ� ����
���ļ�(Performance Frequency)�� ����Ͽ� �ð� ������ �����Ѵ�. ���� ���ļ��� �����
�� ������ ��Ƽ�̵�� Ÿ�̸Ӹ� ����Ѵ�. �� ��� �ð� ������ 0.001��(Millisecond)�̴�. ��
�� ī���Ϳ� ���� �ڼ��� ������ ������ API ������ �����϶�.
*/

CGameTimer::CGameTimer()
{
	//�����ս�ī������ ���ļ�(�ý��� ��Ʈ�ɶ� �����Ǿ��ְ� ������μ����� ���� �Ѱᰰ��.
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequency)) //�����ս� ī������ ���ļ��� �˻��Ѵ�.
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
		m_fTimeScale = 1.0f / m_nPerformanceFrequency;  //�̺κ� �ѹ� Ȯ���ؾ߰ڴ�.
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
 CGameTimer::Tick() ��� �Լ�
��ǻ��(CPU)�� ���� ī����(Performance Counter) �ϵ��� ������ ������ ���� ī����
�� ���� ���ļ�(Performance Frequency)�� ����Ͽ� ���� �ð��� �����ϰ� ������ ��Ƽ��
��� Ÿ�̸Ӹ� ����Ѵ�.
*/
void CGameTimer::Tick(float fLockFPS)
{
	if (m_bHardwareHasPerformanceCounter) //���� ī���Ͱ� ������
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);  //����ð��� 
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}
	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale; //m_fTimeScaledms 1/m_nPerformanceFrequency���߾���(�������ļ��������)
	if (fLockFPS > 0.0f)
	{
		//�� �Լ��� �Ķ����(fLockFPS)�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ��� �Ѵ�.
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
			//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�. 
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}
	//���� �ð��� m_nLastTime�� �����Ѵ�. 
	m_nLastTime = m_nCurrentTime;
	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������ ���� ������ ó�� �ð�
	�� m_fFrameTime[0]�� �����Ѵ�. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)  //fabs�� ���밪
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float)); //memove(������, �ҽ�, 49*size(float))
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT)
			m_nSampleCount++;
	}
	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�.
	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond; //���������ӷ���Ʈ�� �ʴ������Ӽ��� �����Ѵ�.
		m_nFramesPerSecond = 0;  //�ʴ������Ӽ��� 0 ����
		m_fFPSTimeElapsed = 0.0f; //�����ӷ���Ʈ ���ҿ�ð��� 0����
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�.
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++)
		m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0)
		m_fTimeElapsed /= m_nSampleCount;
}

/*
 CGameTimer::GetFrameRate() ��� �Լ�
������ ������ ����Ʈ�� ���ڿ��� ���������� ��ȯ�Ѵ�.
*/
unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�. 
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