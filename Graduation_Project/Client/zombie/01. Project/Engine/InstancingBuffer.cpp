#include "stdafx.h"
#include "InstancingBuffer.h"

#include "Device.h"
#include "StructuredBuffer.h"

#include "Ptr.h"
#include "ResMgr.h"

CInstancingBuffer::CInstancingBuffer()
	: m_bufferID(0)
	, m_iMaxCount(10)
	, m_iAnimInstCount(0)
	, m_pBoneBuffer(nullptr)
	, m_bUpdated(false)
{
	m_pBoneBuffer = new CStructuredBuffer;
}

CInstancingBuffer::~CInstancingBuffer()
{	
	SAFE_DELETE(m_pBoneBuffer);
}

void CInstancingBuffer::init()
{
	D3D12_HEAP_PROPERTIES tHeapProperty = {};

	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC tResDesc = {};

	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = sizeof(tInstancingData) * m_iMaxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	DEVICE->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pInstancingBuffer));
}

void CInstancingBuffer::SetData()
{
	// 갱신 완료 체크
	m_bUpdated = true;

	if (m_vecData.size() > m_iMaxCount)
	{
		Resize((UINT)m_vecData.size());
	}	
		
	// Copy the Matrix data to the buffer.
	UINT8* pDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.
	m_pInstancingBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
	memcpy(pDataBegin, &m_vecData[0], sizeof(tInstancingData) * m_vecData.size());
	m_pInstancingBuffer->Unmap(0, nullptr);

	// Initialize the buffer view.
	m_tBufferView.BufferLocation = m_pInstancingBuffer->GetGPUVirtualAddress();
	m_tBufferView.StrideInBytes = sizeof(tInstancingData);
	m_tBufferView.SizeInBytes = sizeof(tInstancingData) * (UINT)m_vecData.size();
}

void CInstancingBuffer::Resize(UINT _iCount)
{
	SAFE_RELEASE(m_pInstancingBuffer);

	m_iMaxCount = _iCount;

	init();
}

void CInstancingBuffer::Resize_BoneBuffer(UINT _iInstCount, UINT _iBufferSize)
{
	UINT iMaxBufferSize = _iInstCount * _iBufferSize;
	if (m_pBoneBuffer->GetBufferSize() < iMaxBufferSize)
	{
		m_pBoneBuffer->Create(_iBufferSize, _iInstCount, nullptr);
	}
}
