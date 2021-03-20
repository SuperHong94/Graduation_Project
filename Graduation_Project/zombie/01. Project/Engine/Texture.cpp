#include "stdafx.h"
#include "Texture.h"

#include "Device.h"
#include "PathMgr.h"

#include <wincodec.h>

CTexture::CTexture()
	: CResource(RES_TYPE::TEXTURE)
	, m_tDesc{}
	, m_pTex2D(nullptr)		
	, m_pSRV(nullptr)
	, m_pRTV(nullptr)
	, m_pDSV(nullptr)	
	, m_pUAV(nullptr)
	, m_eState(D3D12_RESOURCE_STATE_COMMON)
{
}

CTexture::~CTexture()
{
}

void CTexture::Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
	, const D3D12_HEAP_PROPERTIES & _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag, D3D12_RESOURCE_FLAGS _eResFlag
	, Vec4 _vClearColor)
{	
	m_tDesc.MipLevels = 1;
	m_tDesc.Format = _eFormat;
	m_tDesc.Width = _iWidth;
	m_tDesc.Height = _iHeight;
	m_tDesc.Flags = _eResFlag;
	m_tDesc.DepthOrArraySize = 1;
	m_tDesc.SampleDesc.Count = 1;
	m_tDesc.SampleDesc.Quality = 0;
	m_tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_tDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE* pValue = nullptr;
	m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (_eResFlag & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
		pValue = &depthOptimizedClearValue;
		m_eState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}	
	else if (_eResFlag & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		//m_tDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
		m_eState = D3D12_RESOURCE_STATE_COMMON;
		float arrFloat[4] = { _vClearColor.x, _vClearColor .y, _vClearColor .z, _vClearColor .w};
		CD3DX12_CLEAR_VALUE depthOptimizedClearValue(_eFormat, arrFloat);
		pValue = &depthOptimizedClearValue;
	}

	HRESULT hr = DEVICE->CreateCommittedResource(
		&_HeapProperty,
		_eHeapFlag,
		&m_tDesc,
		m_eState,
		pValue,
		IID_PPV_ARGS(&m_pTex2D));

	if (FAILED(hr))
		assert(nullptr);

	// Texture 를 관리할 View 생성(SRV, RTV, DSV, UAV)
	if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		hr = DEVICE->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV));
		
		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		DEVICE->CreateDepthStencilView(m_pTex2D.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			// RenderTargetView 만들기	
			D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
			tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			tDesc.NumDescriptors = 1;
			tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			tDesc.NodeMask = 0;
			DEVICE->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));
			D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

			DEVICE->CreateRenderTargetView(m_pTex2D.Get(), nullptr, hRTVHeap);
		}
		
		if (_eResFlag & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
			uavHeapDesc.NumDescriptors = 1;
			uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			DEVICE->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_pUAV));

			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pUAV->GetCPUDescriptorHandleForHeapStart();

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};			
			uavDesc.Format = _eFormat;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;			

			DEVICE->CreateUnorderedAccessView(m_pTex2D.Get(), nullptr, &uavDesc, handle);
		}

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
		DEVICE->CreateShaderResourceView(m_pTex2D.Get(), &srvDesc, handle);
	}
}

void CTexture::CreateFromResource(ComPtr<ID3D12Resource> _pTex2D)
{
	m_pTex2D = _pTex2D;
	m_tDesc = _pTex2D->GetDesc();

	// Texture 를 관리할 View 생성(SRV, RTV, DSV)
	HRESULT hr = S_OK;
	if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
		tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		tDesc.NumDescriptors = 1;
		tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		tDesc.NodeMask = 0;
		hr = DEVICE->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pDSV));

		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = m_pDSV->GetCPUDescriptorHandleForHeapStart();
		DEVICE->CreateDepthStencilView(m_pTex2D.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			// RenderTargetView 만들기	
			D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
			tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			tDesc.NumDescriptors = 1;
			tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			tDesc.NodeMask = 0;
			DEVICE->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));
			D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

			DEVICE->CreateRenderTargetView(m_pTex2D.Get(), nullptr, hRTVHeap);
		}

		if (m_tDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE)
		{
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
	}
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
		if (FAILED(LoadFromWICFile(_strFullPath.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, m_Image)))
		{
			assert(nullptr);
		}
	}

	HRESULT hr = CreateTexture(DEVICE.Get(), m_Image.GetMetadata(), &m_pTex2D);
	if (FAILED(hr))
		assert(nullptr);

	m_tDesc = m_pTex2D->GetDesc();

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

	m_tDesc = m_pTex2D->GetDesc();
}

void CTexture::Save(const wstring& _strPath)
{
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;

	const wchar_t* pExt = CPathMgr::GetExt(_strPath.c_str());
	const Image* pImage = m_Image.GetImages();
	if (!wcscmp(pExt, L".dds"))
	{
		SaveToDDSFile(m_Image.GetImages(), m_Image.GetMetadata().arraySize, m_Image.GetMetadata(), DDS_FLAGS::DDS_FLAGS_NONE, strPath.c_str());
	}
	else if (!wcscmp(pExt, L".tga"))
	{
		SaveToTGAFile(*pImage, strPath.c_str());
	}
	else
	{
		SaveToWICFile(*pImage, WIC_FLAGS_NONE, GUID_ContainerFormatPng, strPath.c_str());
	}
	
}

