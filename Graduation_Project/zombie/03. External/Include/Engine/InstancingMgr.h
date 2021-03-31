#pragma once

#include "InstancingBuffer.h"

class CInstancingMgr
{
	SINGLE(CInstancingMgr);
private:
	vector<CInstancingBuffer*>	m_vecInstingBuffer;
	UINT						m_iInstIdx;

public:
	void init();
	void clear();

public:
	UINT GetCurrentInstancingIdx() {return m_iInstIdx;}
	void AddInstancingData(tInstancingData& _tData, bool _bAnim);
	//void AddInstancingBoneMat(CStructuredBuffer* _pBuffer);

	UINT GetInstanceCount(UINT _iInstancingIdx) {
		return m_vecInstingBuffer[_iInstancingIdx]->GetInstanceCount();
	}
		
	CInstancingBuffer* GetInstancingBuffer(long long _iBufferID);
	CInstancingBuffer* AllocBuffer(long long _bufferid);

	void SetData();
};

