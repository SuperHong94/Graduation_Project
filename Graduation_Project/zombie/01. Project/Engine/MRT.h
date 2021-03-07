#pragma once
#include "Entity.h"

#include "Ptr.h"
#include "Texture.h"

struct tRT
{
	Ptr<CTexture> pTarget;
	Vec4		  vClearColor;
};

class CMRT :
	public CEntity
{
private:
	tRT								m_arrRT[8];
	UINT							m_iRTCount;
	Ptr<CTexture>					m_pDSTex;
	ComPtr<ID3D12DescriptorHeap>	m_pRTV;

	D3D12_RESOURCE_BARRIER			m_TargetToRes[8];
	D3D12_RESOURCE_BARRIER			m_ResToTarget[8];

public:
	void Create(UINT _iCount, tRT* _arrRT, Ptr<CTexture> _pDSTex);
	void OMSet(UINT _iCount, UINT _iOffset = 0);
	void OMSet();
	void TargetToResBarrier();
	void ResToTargetBarrier();
	void Clear();
	void Clear(UINT _iRTIdx);

	Ptr<CTexture> GetRTTex(UINT _iIdx) { return m_arrRT[_iIdx].pTarget; }
	Ptr<CTexture> GetDSTex() { return m_pDSTex; }

public:
	CMRT();
	virtual ~CMRT();
};

