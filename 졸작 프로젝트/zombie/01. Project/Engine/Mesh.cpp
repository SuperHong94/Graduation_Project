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
{
}

CMesh::~CMesh()
{
	SAFE_DELETE(m_pVtxSysMem);	

	//SAFE_RELEASE(m_pVB);
	//SAFE_RELEASE(m_pIB);
	//SAFE_RELEASE(m_pLayout);
}

void CMesh::Create(UINT _iBufferSize, BYTE* _pVtxSysMem)
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
	tResDesc.Width = _iBufferSize;
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
	memcpy(pVertexDataBegin, _pVtxSysMem, _iBufferSize);
	m_pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_tVtxView.BufferLocation = m_pVB->GetGPUVirtualAddress();
	m_tVtxView.StrideInBytes = sizeof(VTX);
	m_tVtxView.SizeInBytes = _iBufferSize;
}

void CMesh::render()
{
	// Input Assembler 단계임을 알림	
	CMDLIST->SetGraphicsRootSignature(CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());

	CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMDLIST->IASetVertexBuffers(0, 1, &m_tVtxView);
	CMDLIST->DrawInstanced(3, 1, 0, 0);
}
