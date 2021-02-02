#include "stdafx.h"
#include "Texture.h"

#include "Device.h"

CTexture::CTexture()
	: CResource(RES_TYPE::TEXTURE)
	, m_pTex2D(nullptr)	
	, m_pSRV(nullptr)
{
}

CTexture::~CTexture()
{
}

void CTexture::Load(const wstring & _strFullPath)
{
	// 확장자명 얻기
	wchar_t szExt[50] = L"";	
	_wsplitpath_s(_strFullPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	wstring strExt = szExt;

	if (L".dds" == strExt || L".DDS" == strExt)
	{
		if(FAILED(LoadFromDDSFile(_strFullPath.c_str(), DDS_FLAGS_NONE, nullptr, m_Image)))
		{
			assert(nullptr);
		}
	}
	else if (L".tga" == strExt || L".TGA" == strExt)
	{
		if (FAILED(LoadFromTGAFile(_strFullPath.c_str(), nullptr, m_Image)))
		{
			assert(nullptr);
		}
	}
	else // png, jpg, jpeg, bmp (WIC)
	{
		if (FAILED(LoadFromWICFile(_strFullPath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image)))
		{
			assert(nullptr);
		}
	}

	HRESULT hr = CreateTexture(DEVICE.Get(), m_Image.GetMetadata(), &m_pTex2D);
	if (FAILED(hr))
		assert(nullptr);

	vector<D3D12_SUBRESOURCE_DATA> vecSubresources;
	
	hr = PrepareUpload(DEVICE.Get()
		, m_Image.GetImages()
		, m_Image.GetImageCount()
		, m_Image.GetMetadata()
		, vecSubresources);

	if (FAILED(hr))
		assert(nullptr);

	// upload is implemented by application developer. Here's one solution using <d3dx12.h>
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pTex2D.Get(), 0, static_cast<unsigned int>(vecSubresources.size()));
	
	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC    tDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	ComPtr<ID3D12Resource> textureUploadHeap;
	hr = DEVICE->CreateCommittedResource(
		&tUploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&tDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

	if (FAILED(hr))
		assert(nullptr);
	
	UpdateSubresources(CMDLIST_RES.Get()
		, m_pTex2D.Get()
		, textureUploadHeap.Get()
		, 0, 0
		, static_cast<unsigned int>(vecSubresources.size())
		, vecSubresources.data());

	CDevice::GetInst()->ExcuteResourceLoad();

	// SRV (shader resource view) 만들기
	// SRV 를 저장할 DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Image.GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	DEVICE->CreateShaderResourceView(m_pTex2D.Get(), &srvDesc, m_pSRV->GetCPUDescriptorHandleForHeapStart());	
}

void CTexture::UpdateData(UINT _iShaderType, UINT _iRegisterNum)
{

}

void CTexture::ClearData(UINT _iShaderType, UINT _iRegisterNum)
{
	
}
