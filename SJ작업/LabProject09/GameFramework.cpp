
#include "stdafx.h"
#include "GameFramework.h"
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;
	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dPipelineState = NULL;
	m_pd3dCommandList = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_ppd3dRenderTargetBuffers[i] = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_nRtvDescriptorIncrementSize = 0;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDsvDescriptorHeap = NULL;
	m_nDsvDescriptorIncrementSize = 0;
	m_nSwapChainBufferIndex = 0;
	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;
	m_pScene = NULL;
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszFrameRate, _T("DirectProject("));
}
CGameFramework::~CGameFramework()
{
}


//다음 함수는 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성하는 함수를 호출한다.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();

	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();
	//렌더링할 게임 객체를 생성한다.

	return(true);
}





void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;
	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++) //그래픽카드 찾기
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc); //그래픽카드 정보얻기
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice)))
			break; //하나 만들면 빠져나온다.
	}
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다. 

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //후면버퍼의 픽셀방식
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링 샘플의 개수
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.

	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence); //펜스는 기본펜스로 만든다.
	m_nFenceValues[m_nSwapChainBufferIndex] = 0; //원래 m_nFenceValue = 0;이거였는데 따라하기 06에서 내가 수정함
	//펜스를 생성하고 펜스 값을 0으로 설정한다. 

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다). 이벤트가 실행되면(Signal) 이
	벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/

	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;
	//뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다.

	m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	//씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다. 

	if (pd3dAdapter)
		pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; //기본 명령 큐
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //GPU가 직접 실행할 수 있는 명령버퍼(리스트)
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//직접(Direct) 명령 큐를 생성한다.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//직접(Direct) 명령 할당자를 생성한다.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//직접(Direct) 명령 리스트를 생성한다. 

	hResult = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다. 
}


void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);   //사각형가져오기
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc; //스왑체인1 디스크립터 
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1)); //메모리블록 지우기
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //한픽셀의 포멧설정
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //버퍼를 렌더 타겟으로 사용
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers; //버퍼 개수
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  //버퍼내용을폐기
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;  //투명처리 안함
	dxgiSwapChainDesc.Flags = 0;  //윈도우모드에서만 할것이다.

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60; //분모
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;//분자
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //알아서 비율에 맞게 조정 해줌
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;  //전체화면 True

	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);// CreateSwapChainForHwnd을 로 바꿔줌 
	//스왑체인을 생성한다. 

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//스왑체인3에 있는내용
	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER); //전체화면 알트+엔터로 할건지 안할건지로 설정)
	//“Alt+Enter” 키의 동작을 비활성화한다.
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif


}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps() //렌더타겟, 깊이 스텐실 뷰 디스크립터힙을 만들기
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers; //렌더타겟의 수(서술자의 개수)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// 랜더타겟뷰로 만든다.
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //쉐이더에서 디스크립터에 연결된 리소스를 사용하지 못하게한다.
	d3dDescriptorHeapDesc.NodeMask = 0; //단일 GPU일때는 0으로 한다!
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.

	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.

	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다. 
}


//스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다. 
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();  //힙의 시작을 나타내는 CPU서술자 핸들을 반환
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]); //후면버퍼를 반환하는데 리소스로 사용할 것이라고 알려줌
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle); //null을 한것은 리소스의 형시과 차원을 사용한다는 말이다.
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}


//깊이 스텐실 뷰
void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;  //메모리에 특정한 위치에 정렬하는 기준  0은64kb(디폴트)
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1; //2차원 x,y 평면
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //리소스형식
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;  //레이아웃 모르니깐 알아서 하셈
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //리소스 사용용도는 저장용도로 사용한다.


	D3D12_HEAP_PROPERTIES d3dHeapProperties;  //힙의 속성을 표현,
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;  //GPU는 읽기 쓰기 가능, CPU는 접근 불가
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //알아서해라
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //알아서 해라
	d3dHeapProperties.CreationNodeMask = 1; //다일 GPU어댑터
	d3dHeapProperties.VisibleNodeMask = 1; //다중 어댑터에서 리소스를 볼 수 있는 노드의 집합을 나타내는 비트


	D3D12_CLEAR_VALUE d3dClearValue; //어떻게 지울지 알려줌
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //스텐실이 0~255이나깐 8bit
	d3dClearValue.DepthStencil.Depth = 1.0f; //depth는 원근투영한 결과
	d3dClearValue.DepthStencil.Stencil = 0;


	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_HEAP_FLAG_NONE 알아서 하셈
	//D3D12_RESOURCE_STATES를 D3D12_RESOURCE_STATE_DEPTH_WRITE이것으로 한다는 것은 깊이 버퍼에 쓸때
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	//깊이-스텐실 버퍼를 생성한다.




	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
	//깊이-스텐실 버퍼 뷰를 생성한다. 
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//카메라 객체를 생성하여 뷰포트, 씨저 사각형, 투영 변환 행렬, 카메라 변환 행렬을 생성하고 설정한다. 
	m_pCamera = new CCamera();
	m_pCamera->SetViewport(0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	m_pCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	//씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성한다.
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	//씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다.
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	//그래픽 명령 리스트들이 모두 실행될 때까지 기다린다.
	WaitForGpuComplete();
	//그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다. 
	if (m_pScene) m_pScene->ReleaseUploadBuffers();

}
void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM
	wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{

		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
		case VK_UP:
		{
			CGameObject* Player = m_pScene->GetppObjects()[0];
			XMFLOAT3 pos = Player->GetPos();
			Player->SetPos(pos.x, pos.y + 1.f, pos.z);
		}
		break;
		case VK_DOWN:
		{
			CGameObject* Player = m_pScene->GetppObjects()[0];
			XMFLOAT3 pos = Player->GetPos();
			Player->SetPos(pos.x, pos.y -1.f, pos.z);
		}
		break;
		case VK_LEFT:
		{
			CGameObject* Player = m_pScene->GetppObjects()[0];
			XMFLOAT3 pos = Player->GetPos();
			Player->SetPos(pos.x-1.f, pos.y, pos.z);
		}
		break;
		case VK_RIGHT:
		{
			CGameObject* Player = m_pScene->GetppObjects()[0];
			XMFLOAT3 pos = Player->GetPos();
			Player->SetPos(pos.x+1.f, pos.y, pos.z);
		}
		break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:

	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}
void CGameFramework::ProcessInput()
{
}
void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	//CPU 펜스의 값을 증가한다. 

	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}



void CGameFramework::FrameAdvance()
{
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다.
	m_GameTimer.Tick(0.0f);

	ProcessInput();

	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	//명령 할당자와 명령 리스트를 리셋한다.

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //1학기때에는 이것씀 리소스전이(사용 변화)를 나타내는 전이 장벽 
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;   //이거 뭐냐?
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;  //이전상태
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //이후상태
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; //우리는 리소스전체
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다. 프리젠트가 끝나면 렌더 타겟 버퍼의 상태는 프리젠트 상태
	(D3D12_RESOURCE_STATE_PRESENT)에서 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바
	뀔 것이다.*/

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
	//현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산한다. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//깊이-스텐실 서술자의 CPU 주소를 계산한다.

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	//렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM)에 연결한다. //렌더링 코드는 여기에 추가될 것이다.

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	//원하는 색상으로 렌더 타겟(뷰)을 지운다. 

	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//원하는 값으로 깊이-스텐실(뷰)을 지운다. 


	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);  //씬그리기



	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다. GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면 렌더 타겟
	의 상태는 프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.*/

	hResult = m_pd3dCommandList->Close();
	//명령 리스트를 닫힌 상태로 만든다.

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	//명령 리스트를 명령 큐에 추가하여 실행한다.
	WaitForGpuComplete();
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다. 

	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;

	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);//따라하기 4이후로 >Present1(1, 0, &dxgiPresentParameters)에서  present(0,0)으로 바뀜
	/*스왑체인을 프리젠트한다. 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인
	덱스가 바뀔 것이다.*/


	/*현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력한다. m_pszBuffer 문자열이
"LapProject ("으로 초기화되었으므로 (m_pszFrameRate+12)에서부터 프레임 레이트를 문자열로 출력
하여 “ FPS)” 문자열과 합친다. ::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate+12), 37, 10);
::wcscat_s((m_pszFrameRate+12), 37, _T(" FPS)"));
*/

	MoveToNextFrame(); //다음프레임으로!

	m_GameTimer.GetFrameRate(m_pszFrameRate + 14, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate); //윈도우 제목을 m_pszFrameRate

}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();
	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.

	ReleaseObjects();
	//게임 객체(게임 월드 객체)를 소멸한다. 

	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dRtvDescriptorHeap) //힙
		m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dDepthStencilBuffer) //스텐실버퍼
		m_pd3dDepthStencilBuffer->Release();

	if (m_pd3dDsvDescriptorHeap)
		m_pd3dDsvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator)
		m_pd3dCommandAllocator->Release();

	if (m_pd3dCommandQueue)
		m_pd3dCommandQueue->Release();

	if (m_pd3dPipelineState)
		m_pd3dPipelineState->Release();

	if (m_pd3dCommandList)
		m_pd3dCommandList->Release();

	if (m_pd3dFence)
		m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);

	if (m_pdxgiSwapChain)
		m_pdxgiSwapChain->Release();

	if (m_pd3dDevice)
		m_pd3dDevice->Release();

	if (m_pdxgiFactory)
		m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

//풀스크린 만들기
void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL); //매개변수설명(현재 스웝체인이 풀스크린 모드인지 확인,출력 모니터는 자동선택)
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters; //Describes a display mode.
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;// 후면버퍼 머비
	dxgiTargetParameters.Height = m_nWndClientHeight; //후면버퍼 높이
	dxgiTargetParameters.RefreshRate.Numerator = 60; //분모
	dxgiTargetParameters.RefreshRate.Denominator = 1;//분자
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //모니터에 그림을 어캐 맞출껀지 주는 옵션
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters); //출력타겟의 크기를 변경

	//m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL); //내가추가한내용
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i]) //따라하기 내용은 m_ppd3dSwapChainBackBuffers 이거였음
			m_ppd3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags); //스왑체인의 후면 버퍼의 크기, 형식, 개수를 변경

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();

}