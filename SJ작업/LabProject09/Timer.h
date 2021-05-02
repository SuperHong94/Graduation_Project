#pragma once
const ULONG MAX_SAMPLE_COUNT = 50;// 50ȸ�� ������ ó�� �ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f); //Ÿ�̸��� �ð��� �����Ѵ�.
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);//������ ����Ʈ�� ��ȯ�Ѵ�.
	float GetTimeElapsed(); //�������� ��� ��� �ð��� ��ȯ�Ѵ�.

private:
	bool m_bHardwareHasPerformanceCounter; //��ǻ�Ͱ� performance Couter�� ������ �ִ°�.
	float m_fTimeScale; //Scale Counter�� ��
	float m_fTimeElapsed; //������ ������ ���� ������ �ð�
	__int64 m_nCurrentTime; //������ �ð�
	__int64 m_nLastTime; //������ �������� �ð�
	__int64 m_nPerformanceFrequency; //��ǻ���� performace Frequency
	float m_fFrameTime[MAX_SAMPLE_COUNT]; //�����ӽð��� �����ϱ� ���� �迭
	ULONG m_nSampleCount; //������ ������ Ƚ��
	unsigned long m_nCurrentFrameRate; //������ ������ ����Ʈ
	unsigned long m_nFramesPerSecond; //�ʴ� ������ ��
	float m_fFPSTimeElapsed; //������ ����Ʈ ��� �ҿ� �ð�

};

