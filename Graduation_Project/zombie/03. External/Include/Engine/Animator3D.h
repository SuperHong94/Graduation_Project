#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Texture.h"

class CAnimator3D 
	: public CComponent
{
private:
	const vector<tMTBone>*		m_pVecBones;
	const vector<tMTAnimClip>*	m_pVecClip;

	vector<float>				m_vecClipUpdateTime;
	vector<Matrix>				m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
	int							m_iFrameCount; // 30
	double						m_dCurTime;
	int							m_iCurClip; // 클립 인덱스
	Ptr<CTexture>				m_pBoneTex; // mesh

public:
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void SetBoneTex(Ptr<CTexture> _pBoneTex) { m_pBoneTex = _pBoneTex; }
	Ptr<CTexture> GetBornTex() { return m_pBoneTex; }
	void UpdateData();

	void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

	vector<Matrix>& GetFinalBoneMat() { return m_vecFinalBoneMat; }

public:
	virtual void awake() {};
	virtual void start() {};
	virtual void update();
	virtual void lateupdate();
	virtual void finalupdate();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);
	CAnimator3D* Clone() { return new CAnimator3D(*this); }

public:
	CAnimator3D();
	virtual ~CAnimator3D();
};

