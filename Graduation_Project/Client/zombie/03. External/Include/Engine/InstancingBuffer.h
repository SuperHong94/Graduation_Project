#pragma once
#include "global.h"

#include "Ptr.h"
#include "Texture.h"

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int    iRowIdx;
};

class CTexture;
class CStructuredBuffer;

class CInstancingBuffer
{
private:
	long long					m_bufferID;
	ComPtr<ID3D12Resource>		m_pInstancingBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_tBufferView;

	UINT						m_iMaxCount;
	vector<tInstancingData>		m_vecData;

	int							m_iAnimInstCount;
	CStructuredBuffer*			m_pBoneBuffer;

	bool						m_bUpdated;

public:
	void init();
	void Clear() 
	{
		m_vecData.clear(); 		
		m_iAnimInstCount = 0; 
		m_bUpdated = false;
	}
	void AddInstancingData(tInstancingData& _tData, bool _bAnim) 
	{
		m_vecData.push_back(_tData); 
		if (_bAnim)
			++m_iAnimInstCount;
	}

	UINT GetInstanceCount() { return (UINT)m_vecData.size(); }
	ComPtr<ID3D12Resource> GetBuffer() { return m_pInstancingBuffer; }
	CStructuredBuffer* GetBoneBuffer() { return m_pBoneBuffer; }
	D3D12_VERTEX_BUFFER_VIEW* GetBufferView() { return &m_tBufferView; }
	int GetAnimInstancingCount() { return m_iAnimInstCount; };

	void SetData();

	void SetBufferID(long long _bufferid) { m_bufferID = _bufferid; }
	long long GetBufferID() { return m_bufferID; }
	bool BeUpdated() { return m_bUpdated; }

private:
	void Resize(UINT _iCount);

public:
	void Resize_BoneBuffer(UINT _iInstCount, UINT _iBufferSize);

public:
	CInstancingBuffer();
	~CInstancingBuffer();
};