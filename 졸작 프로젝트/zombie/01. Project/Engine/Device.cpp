#include "stdafx.h"
#include "Device.h"


CDevice::CDevice()
	: m_pDevice(nullptr)
	, m_pFence(nullptr)
	, m_pFactory(nullptr)
	, m_iCurTargetIdx(0)
	, m_hFenceEvent(nullptr)
	, m_iFenceValue(0)
{
}

CDevice::~CDevice()
{		
	WaitForFenceEvent();
	CloseHandle(m_hFenceEvent);
}

int CDevice::init(HWND _hWnd, const tResolution & _res, bool _bWindow)
{
	m_hWnd = _hWnd;
	m_tResolution = _res;
	m_bWindowed = _bWindow;

	UINT iFlag = 0;

#ifdef _DEBUG
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDbgCtrl));
	m_pDbgCtrl->EnableDebugLayer();
#endif

	CreateDXGIFactory(IID_PPV_ARGS(&m_pFactory));
	   	 
	// CreateDevice
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));
	
	// CreateFence
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	m_iFenceValue = 1;

	// Create an event handle to use for frame synchronization.
	m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_hFenceEvent == nullptr)
	{
		assert(nullptr);
	}

	// Command Queue 	
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCmdQueue));

	// Create Command Allocator
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCmdAlloc));

	// Create the command list.
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_pCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListGraphic));

	m_pCmdListGraphic->Close();	

	// SwapChain �����
	CreateSwapChain();

	// View �����
	CreateView();

	// ViewPort �����
	CreateViewPort();
	
	// Empty Signature �����
	// ��Ʈ ���� 
	// �׸��� ȣ�� ���� �ش� �ڿ��� ���������ο� ���� �ڷ��̸�,
	// ��� ������ ���̴���, �Ǵ� ���̴� �ڿ����� ��� �� �������� ���������� ����Ѵ�.
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 0;
	sigDesc.pParameters = nullptr;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pStaticSamplers = nullptr; // ��� �� Sampler ����
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �Է� ������ �ܰ�

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_arrSig[(UINT)ROOT_SIG_TYPE::RENDER]));
	
	return S_OK;
}

void CDevice::render_start(float(&_arrFloat)[4])
{
	// �׸��� �غ�
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	m_pCmdAlloc->Reset();

	m_pCmdListGraphic->Reset(m_pCmdAlloc.Get(), nullptr);

	// �ʿ��� ���� ����	
	m_pCmdListGraphic->RSSetViewports(1, &m_tVP);
	m_pCmdListGraphic->RSSetScissorRects(1, &m_tScissorRect);

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// ��¿���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �ٽ� ����۷� ����
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	hRTVHandle.ptr += m_iCurTargetIdx * m_iRTVHeapSize;
	m_pCmdListGraphic->OMSetRenderTargets(1, &hRTVHandle, FALSE, nullptr);	
}

void CDevice::render_present()
{
	// Indicate that the back buffer will now be used to present.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// ����ۿ���
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �ٽ� ������� ����

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);
	m_pCmdListGraphic->Close();

	// Ŀ�ǵ� ����Ʈ ����	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListGraphic.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	m_pSwapChain->Present(0, 0);

	WaitForFenceEvent();

	// ����� Ÿ�� �ε��� ����
	m_iCurTargetIdx == 0 ? m_iCurTargetIdx = 1 : m_iCurTargetIdx = 0;
}

void CDevice::WaitForFenceEvent()
{
	// Signal and increment the fence value.
	const UINT64 fence = m_iFenceValue;
	m_pCmdQueue->Signal(m_pFence.Get(), fence);
	m_iFenceValue++;
	
	UINT64 a = m_pFence->GetCompletedValue();
	// Wait until the previous frame is finished.
	if (a < fence)
	{
		m_pFence->SetEventOnCompletion(fence, m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CDevice::CreateSwapChain()
{
	// ��� �����츦 �����Ͽ�, ȭ�� ���ſ����� ���
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	tDesc.BufferCount = 2; // dx12 �� ���� ī��Ʈ�� 2�� �־��ش�

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ �ȼ� ����(�ȼ��� 4����Ʈ
	tDesc.BufferDesc.Width = (UINT)m_tResolution.fWidth;  // ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Height = (UINT)m_tResolution.fHeight;// ������ �ػ�(������ �ػ󵵶� ��ġ���ѳ���)
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // ������� �ػ� ����ġ�� ȭ�� �����ϸ��� �Ұ�����
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �ȼ��� ��ĵ���� ����
	tDesc.BufferDesc.RefreshRate.Numerator = 100; // ȭ�� ���� ����
	tDesc.BufferDesc.RefreshRate.Denominator = 1;    // ȭ�� ���� ���� 

	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ��� Ÿ�� �뵵�� ���۸� �����.
	tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	
	tDesc.OutputWindow = m_hWnd;	// ��� ������
	tDesc.Windowed = m_bWindowed;   // â ��� or ��üȭ�� ���
	tDesc.SampleDesc.Count = 1;		// ��Ƽ ���� ��� ����
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� �ĸ� ���� ��ü �� ���� ������ ���� ����
		
	HRESULT hr = m_pFactory->CreateSwapChain(m_pCmdQueue.Get(), &tDesc, &m_pSwapChain);
}

void CDevice::CreateView()
{
	// RenderTargetViewHeap �� �޸� ������
	m_iRTVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// dx12 �������� ���� RenderTargetView ����
	// Descriptor Heap(������ ��) �� Dx11 �� RTV, DSV, UAV, SRV �� ���� ��ü

	// RenderTargetView �����	
	D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tDesc.NumDescriptors = 2;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;
	m_pDevice->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));

	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	for (UINT i = 0; i < 2; i++)
	{
		// ������ SwapChain ���� ���۸� ������
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));

		// �ش� ���۷� RenderTarvetView ������
		m_pDevice->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, hRTVHeap);
		hRTVHeap.ptr += m_iRTVHeapSize; // Offset ����
	}
}

void CDevice::CreateViewPort()
{
	// DirectX �� �׷��� ȭ�� ũ�⸦ �����Ѵ�.
	m_tVP = D3D12_VIEWPORT{ 0.f, 0.f, m_tResolution.fWidth, m_tResolution.fHeight, 0.f, 1.f };	
	m_tScissorRect = D3D12_RECT{0, 0, (LONG)m_tResolution.fWidth, (LONG)m_tResolution.fHeight };
}
