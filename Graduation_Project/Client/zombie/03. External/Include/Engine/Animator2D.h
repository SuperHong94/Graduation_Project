#pragma once
#include "Component.h"


#include "Ptr.h"
#include "Texture.h"

class CAnimation2D;

class CAnimator2D :
	public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*				m_pCurAnim;
	bool						m_bRepeat;
	bool						m_bPlay;

public:	
	virtual void finalupdate();

public:
	bool AddAnimation(const wstring& _strAnimKey, Ptr<CTexture> _pTex, Vec2 _vLT, Vec2 _vGap, int _iFrmCount, float _fDuration);
	void PlayAnimation(const wstring& _strAnimKey, bool _bRepeat);
	void PlayAnimation() { if(nullptr != m_pCurAnim) m_bPlay = true; }
	void PauseAnimation() { m_bPlay = false; }
	void StopAnimation();
	bool IsPlay() { return m_bPlay; }
	bool IsRepeat() { return m_bRepeat; }

	CAnimation2D* FindAnimation(const wstring& _strKey);
	void UpdateData();
	//static void ClearData();

	CAnimation2D* GetCurAnimation() {return m_pCurAnim;	}
	const map<wstring, CAnimation2D*>& GetAnimations() { return m_mapAnim; }

public:
	CLONE(CAnimator2D);
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CAnimator2D();
	CAnimator2D(const CAnimator2D& _origin);
	virtual ~CAnimator2D();
};

