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

	// SwapChain 만들기
	CreateSwapChain();

	// View 만들기
	CreateView();

	// ViewPort 만들기
	CreateViewPort();
	
	// Empty Signature 만들기
	// 루트 서명 
	// 그리기 호출 전에 해당 자원이 파이프라인에 묶일 자료이며,
	// 어느 시점에 묶이는지, 또는 쉐이더 자원으로 사용 시 레지스터 대응정보를 기술한다.
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 0;
	sigDesc.pParameters = nullptr;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pStaticSamplers = nullptr; // 사용 될 Sampler 정보
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_arrSig[(UINT)ROOT_SIG_TYPE::RENDER]));
	
	return S_OK;
}

void CDevice::render_start(float(&_arrFloat)[4])
{
	// 그리기 준비
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	m_pCmdAlloc->Reset();

	m_pCmdListGraphic->Reset(m_pCmdAlloc.Get(), nullptr);

	// 필요한 상태 설정	
	m_pCmdListGraphic->RSSetViewports(1, &m_tVP);
	m_pCmdListGraphic->RSSetScissorRects(1, &m_tScissorRect);

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = m_RenderTargets[m_iCurTargetIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 출력에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 다시 백버퍼로 지정
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
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 백버퍼에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 다시 출력으로 지정

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);
	m_pCmdListGraphic->Close();

	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListGraphic.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	m_pSwapChain->Present(0, 0);

	WaitForFenceEvent();

	// 백버퍼 타겟 인덱스 변경
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
	// 출력 윈도우를 지정하여, 화면 갱신역할을 담당
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	tDesc.BufferCount = 2; // dx12 는 버퍼 카운트는 2를 넣어준다

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 픽셀 포멧(픽셀당 4바이트
	tDesc.BufferDesc.Width = (UINT)m_tResolution.fWidth;  // 버퍼의 해상도(윈도우 해상도랑 일치시켜놓음)
	tDesc.BufferDesc.Height = (UINT)m_tResolution.fHeight;// 버퍼의 해상도(윈도우 해상도랑 일치시켜놓음)
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // 윈도우와 해상도 불일치시 화면 스케일링을 할것인지
	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 픽셀의 스캔라인 순서
	tDesc.BufferDesc.RefreshRate.Numerator = 100; // 화면 갱신 비율
	tDesc.BufferDesc.RefreshRate.Denominator = 1;    // 화면 갱신 비율 

	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 출력 타겟 용도로 버퍼를 만든다.
	tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	
	tDesc.OutputWindow = m_hWnd;	// 출력 윈도우
	tDesc.Windowed = m_bWindowed;   // 창 모드 or 전체화면 모드
	tDesc.SampleDesc.Count = 1;		// 멀티 샘플 사용 안함
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
		
	HRESULT hr = m_pFactory->CreateSwapChain(m_pCmdQueue.Get(), &tDesc, &m_pSwapChain);
}

void CDevice::CreateView()
{
	// RenderTargetViewHeap 의 메모리 사이즈
	m_iRTVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// dx12 설명자힙 으로 RenderTargetView 생성
	// Descriptor Heap(설명자 힙) 이 Dx11 의 RTV, DSV, UAV, SRV 를 전부 대체

	// RenderTargetView 만들기	
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
		// 생성된 SwapChain 에서 버퍼를 가져옴
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));

		// 해당 버퍼로 RenderTarvetView 생성함
		m_pDevice->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, hRTVHeap);
		hRTVHeap.ptr += m_iRTVHeapSize; // Offset 증가
	}
}

void CDevice::CreateViewPort()
{
	// DirectX 로 그려질 화면 크기를 설정한다.
	m_tVP = D3D12_VIEWPORT{ 0.f, 0.f, m_tResolution.fWidth, m_tResolution.fHeight, 0.f, 1.f };	
	m_tScissorRect = D3D12_RECT{0, 0, (LONG)m_tResolution.fWidth, (LONG)m_tResolution.fHeight };
}
