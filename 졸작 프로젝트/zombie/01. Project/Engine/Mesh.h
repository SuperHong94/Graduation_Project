#pragma once
#include "Resource.h"


class CMesh :
	public CResource
{
private:
	ComPtr<ID3D12Resource>		m_pVB;
	D3D12_VERTEX_BUFFER_VIEW	m_tVtxView; // 버텍스 버퍼 정보 구조체(stride, total size, 가상 GPU 주소)

	UINT						m_iVtxSize;
	UINT						m_iVtxCount;
	
	void*						m_pVtxSysMem;	


public:
	void Create(UINT _iBufferSize, BYTE* _pVtxSysMem);
	void render();

public:
	CMesh();
	virtual ~CMesh();
};

