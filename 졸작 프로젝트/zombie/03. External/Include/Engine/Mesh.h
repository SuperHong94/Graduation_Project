#pragma once
#include "Resource.h"


class CMesh :
	public CResource
{
private:
	ComPtr<ID3D12Resource>		m_pVB;
	D3D12_VERTEX_BUFFER_VIEW	m_tVtxView; // ���ؽ� ���� ���� ����ü(stride, total size, ���� GPU �ּ�)

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

