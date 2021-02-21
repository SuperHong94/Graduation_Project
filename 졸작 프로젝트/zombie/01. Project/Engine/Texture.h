#pragma once
#include "Resource.h"

#include <DirectTex/DirectXTex.h>
#include <DirectTex/DirectXTex.inl>

#ifdef _DEBUG
#pragma comment(lib, "DirectTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectTex\\DirectXTex.lib")
#endif


class CTexture :
	public CResource
{
private:
	ScratchImage			 	 m_Image;
	D3D12_RESOURCE_DESC			 m_tDesc;
	ComPtr<ID3D12Resource>		 m_pTex2D;
	ComPtr<ID3D12DescriptorHeap> m_pSRV;
	ComPtr<ID3D12DescriptorHeap> m_pRTV;
	ComPtr<ID3D12DescriptorHeap> m_pDSV;


public:
	void Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
		, const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
		, D3D12_RESOURCE_FLAGS _eResFlag, Vec4 _vClearClolr = Vec4());

	void CreateFromResource(ComPtr<ID3D12Resource> _pTex2D);

	ComPtr<ID3D12Resource> GetTex2D() { return m_pTex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return m_pSRV; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return m_pRTV; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return m_pDSV; }

	float Width() { return (float)m_Image.GetMetadata().width; }
	float Height() { return (float)m_Image.GetMetadata().height;}

	virtual void Load(const wstring& _strFullPath);
	virtual void Save(const wstring& _strPath/*최종 경로*/);

public:
	CTexture();
	virtual ~CTexture();
};

