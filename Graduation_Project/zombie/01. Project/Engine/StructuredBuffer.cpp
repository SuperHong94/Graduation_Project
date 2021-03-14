#include "stdafx.h"
#include "StructuredBuffer.h"

#include "Device.h"

CStructuredBuffer::CStructuredBuffer()
	: m_pBuffer(nullptr)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
	, m_iElementSize(0)
	, m_iElementCount(0)
	, m_eResState(D3D12_RESOURCE_STATE_COMMON)
{
}

CStructuredBuffer::~CStructuredBuffer()
{	
}

void CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, void* _pSysmem)
{
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;	
	m_eResState = D3D12_RESOURCE_STATE_COMMON;

	// 滚欺 Desc 累己
	D3D12_RESOURCE_DESC tBufferDesc = {};

	tBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tBufferDesc.Width = m_iElementSize * m_iElementCount;
	tBufferDesc.Height = 1;
	tBufferDesc.DepthOrArraySize = 1;
	tBufferDesc.MipLevels = 1;
	tBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	tBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	tBufferDesc.SampleDesc.Count = 1;
	tBufferDesc.SampleDesc.Quality = 0;

	// Buffer Create
	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	DEVICE->CreateCommittedResource(
		&tUploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&tBufferDesc,
		m_eResState,
		nullptr,
		IID_PPV_ARGS(&m_pBuffer));	   	

	// UAV 积己
	// UAV 甫 历厘且 DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
	uavHeapDesc.NumDescriptors = 1;
	uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DEVICE->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_pUAV));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pUAV->GetCPUDescriptorHandleForHeapStart();

	// UAV DescriptorHeap 俊 UAV 积己
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_iElementCount;
	uavDesc.Buffer.StructureByteStride = m_iElementSize;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	DEVICE->CreateUnorderedAccessView(m_pBuffer.Get(), nullptr, &uavDesc, handle);

	
	// SRV 积己
	// SRV 甫 历厘且 DescriptorHeap Create
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

	handle = m_pSRV->GetCPUDescriptorHandleForHeapStart();

	// SRV DescriptorHeap 俊 SRV 积己
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;	
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iElementCount;
	srvDesc.Buffer.StructureByteStride = m_iElementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	DEVICE->CreateShaderResourceView(m_pBuffer.Get(), &srvDesc, handle);
}

void CStructuredBuffer::UpdateData(TEXTURE_REGISTER _iRegister)
{
	CDevice::GetInst()->SetBufferToRegister(this, _iRegister);
}

void CStructuredBuffer::UpdateData_CS(TEXTURE_REGISTER _iRegister)
{
	CDevice::GetInst()->SetBufferToSRVRegister_CS(this, _iRegister);
}

void CStructuredBuffer::UpdateRWData(UAV_REGISTER _iRegister)
{
	CDevice::GetInst()->SetBufferToUAVRegister_CS(this, _iRegister);
}