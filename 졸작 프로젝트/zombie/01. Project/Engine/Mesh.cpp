#include "stdafx.h"
#include "Mesh.h"

#include "Device.h"

CMesh::CMesh()
	: CResource(RES_TYPE::MESH)
	, m_pVB(nullptr)
	, m_tVtxView{}	
	, m_iVtxSize(0)
	, m_iVtxCount(0)	
    //, m_eIdxFormat(DXGI_FORMAT::DXGI_FORMAT_R32_UINT)
    //, m_iIdxCount(0)
    //, m_eIdxUsage(D3D11_USAGE::D3D11_USAGE_DEFAULT)
    , m_pVtxSysMem(nullptr)
    , m_pIdxSysMem(nullptr)
{
}

CMesh::~CMesh()
{
	SAFE_DELETE(m_pVtxSysMem);
	SAFE_DELETE(m_pIdxSysMem);

	//SAFE_RELEASE(m_pVB);
	//SAFE_RELEASE(m_pIB);
	//SAFE_RELEASE(m_pLayout);
}

void CMesh::Create(UINT _iVtxSize, UINT _iVtxCount, BYTE* _pVtxSysMem
	, DXGI_FORMAT _eIdxFormat, UINT _iIdxCount, BYTE* _pIdxSysMem)
{
	m_iVtxSize = _iVtxSize;
	m_iVtxCount = _iVtxCount;

	m_eIdxFormat = _eIdxFormat;
	m_iIdxCount = _iIdxCount;


	D3D12_HEAP_PROPERTIES tHeapProperty = {};
		
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;
	
	D3D12_RESOURCE_DESC tResDesc = {};

	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = m_iVtxSize * m_iVtxCount;
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
		IID_PPV_ARGS(&m_pVB));

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	m_pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, _pVtxSysMem, (tResDesc.Width * tResDesc.Height) );
	m_pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_tVtxView.BufferLocation = m_pVB->GetGPUVirtualAddress();
	m_tVtxView.StrideInBytes = sizeof(VTX);
	m_tVtxView.SizeInBytes = (UINT)tResDesc.Width;
	   
	// IdxBuffer
	tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = GetSizeofFormat(m_eIdxFormat) * m_iIdxCount;
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
		IID_PPV_ARGS(&m_pIB));

	// Copy the Index data to the Index buffer.
	UINT8* pIdxDataBegin = nullptr;
	readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	m_pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
	memcpy(pIdxDataBegin, _pIdxSysMem, (tResDesc.Width * tResDesc.Height) );
	m_pIB->Unmap(0, nullptr);

	// Initialize the Index buffer view.
	m_tIdxView.BufferLocation = m_pIB->GetGPUVirtualAddress();
	m_tIdxView.Format = DXGI_FORMAT_R32_UINT;
	m_tIdxView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

	CDevice::GetInst()->WaitForFenceEvent();
}

void CMesh::render()
{
	CDevice::GetInst()->UpdateTable();	
		
	CMDLIST->IASetVertexBuffers(0, 1, &m_tVtxView);
	CMDLIST->IASetIndexBuffer(&m_tIdxView);

	//CMDLIST->DrawInstanced(3, 1, 0, 0);
	CMDLIST->DrawIndexedInstanced(m_iIdxCount, 1, 0, 0, 0);
}
