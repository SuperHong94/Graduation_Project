#pragma once

class CTimeMgr
{
private:
	LARGE_INTEGER	m_llOldCount;
	LARGE_INTEGER   m_llCurCount;
	LARGE_INTEGER   m_llFrequency;

	float			m_fDeltaTime;
	float			m_fAccTime;
	float			m_fFPS;

	bool			m_bFreeze; // DT 가 과하게 넘어갈때 동결

public:
	void init();
	void update();	

public:
	float GetDeltaTime() { return m_fDeltaTime;}
	void DTFreeze(){m_bFreeze = true;}
	SINGLE(CTimeMgr);
};

