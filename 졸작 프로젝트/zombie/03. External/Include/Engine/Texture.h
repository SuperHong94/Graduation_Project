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
	ComPtr<ID3D12Resource>		 m_pTex2D;
	ComPtr<ID3D12DescriptorHeap> m_pSRV;



public:
	virtual void Load(const wstring& _strFullPath);

	virtual void Save(const wstring& _strPath/*최종 경로*/);
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return m_pSRV; }

	float Width() { return (float)m_Image.GetMetadata().width; }
	float Height() { return (float)m_Image.GetMetadata().height;}

public:
	CTexture();
	virtual ~CTexture();
};

