
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


//���� �Լ��� ���� ���α׷��� ����Ǿ� �� �����찡 �����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D ����̽�, ��� ť�� ��� ����Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();

	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();
	//�������� ���� ��ü�� �����Ѵ�.

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
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++) //�׷���ī�� ã��
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc); //�׷���ī�� �������
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice)))
			break; //�ϳ� ����� �������´�.
	}
	//��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�. 

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //�ĸ������ �ȼ����
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø� ������ ����
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.

	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence); //�潺�� �⺻�潺�� �����.
	m_nFenceValues[m_nSwapChainBufferIndex] = 0; //���� m_nFenceValue = 0;�̰ſ��µ� �����ϱ� 06���� ���� ������
	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�. 

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�). �̺�Ʈ�� ����Ǹ�(Signal) ��
	��Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/

	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;
	//����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	//���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�. 

	if (pd3dAdapter)
		pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; //�⺻ ��� ť
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //GPU�� ���� ������ �� �ִ� ��ɹ���(����Ʈ)
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//����(Direct) ��� ť�� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//����(Direct) ��� �Ҵ��ڸ� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//����(Direct) ��� ����Ʈ�� �����Ѵ�. 

	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����. 
}


void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);   //�簢����������
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc; //����ü��1 ��ũ���� 
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1)); //�޸𸮺�� �����
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //���ȼ��� ���伳��
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //���۸� ���� Ÿ������ ���
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers; //���� ����
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  //���۳��������
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;  //����ó�� ����
	dxgiSwapChainDesc.Flags = 0;  //�������忡���� �Ұ��̴�.

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60; //�и�
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;//����
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //�˾Ƽ� ������ �°� ���� ����
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;  //��üȭ�� True

	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);// CreateSwapChainForHwnd�� �� �ٲ��� 
	//����ü���� �����Ѵ�. 

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//����ü��3�� �ִ³���
	//����ü���� ���� �ĸ���� �ε����� �����Ѵ�.

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER); //��üȭ�� ��Ʈ+���ͷ� �Ұ��� ���Ұ����� ����)
	//��Alt+Enter�� Ű�� ������ ��Ȱ��ȭ�Ѵ�.
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif


}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps() //����Ÿ��, ���� ���ٽ� �� ��ũ�������� �����
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers; //����Ÿ���� ��(�������� ����)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// ����Ÿ�ٺ�� �����.
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //���̴����� ��ũ���Ϳ� ����� ���ҽ��� ������� ���ϰ��Ѵ�.
	d3dDescriptorHeapDesc.NodeMask = 0; //���� GPU�϶��� 0���� �Ѵ�!
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.

	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.

	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�. 
}


//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����Ѵ�. 
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();  //���� ������ ��Ÿ���� CPU������ �ڵ��� ��ȯ
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]); //�ĸ���۸� ��ȯ�ϴµ� ���ҽ��� ����� ���̶�� �˷���
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle); //null�� �Ѱ��� ���ҽ��� ���ð� ������ ����Ѵٴ� ���̴�.
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}


//���� ���ٽ� ��
void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;  //�޸𸮿� Ư���� ��ġ�� �����ϴ� ����  0��64kb(����Ʈ)
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1; //2���� x,y ���
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //���ҽ�����
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;  //���̾ƿ� �𸣴ϱ� �˾Ƽ� �ϼ�
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //���ҽ� ���뵵�� ����뵵�� ����Ѵ�.


	D3D12_HEAP_PROPERTIES d3dHeapProperties;  //���� �Ӽ��� ǥ��,
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;  //GPU�� �б� ���� ����, CPU�� ���� �Ұ�
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //�˾Ƽ��ض�
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //�˾Ƽ� �ض�
	d3dHeapProperties.CreationNodeMask = 1; //���� GPU�����
	d3dHeapProperties.VisibleNodeMask = 1; //���� ����Ϳ��� ���ҽ��� �� �� �ִ� ����� ������ ��Ÿ���� ��Ʈ


	D3D12_CLEAR_VALUE d3dClearValue; //��� ������ �˷���
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //���ٽ��� 0~255�̳��� 8bit
	d3dClearValue.DepthStencil.Depth = 1.0f; //depth�� ���������� ���
	d3dClearValue.DepthStencil.Stencil = 0;


	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_HEAP_FLAG_NONE �˾Ƽ� �ϼ�
	//D3D12_RESOURCE_STATES�� D3D12_RESOURCE_STATE_DEPTH_WRITE�̰����� �Ѵٴ� ���� ���� ���ۿ� ����
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	//����-���ٽ� ���۸� �����Ѵ�.




	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
	//����-���ٽ� ���� �並 �����Ѵ�. 
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//ī�޶� ��ü�� �����Ͽ� ����Ʈ, ���� �簢��, ���� ��ȯ ���, ī�޶� ��ȯ ����� �����ϰ� �����Ѵ�. 
	m_pCamera = new CCamera();
	m_pCamera->SetViewport(0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	m_pCamera->GenerateViewMatrix(XMFLOAT3(0.0f, 15.0f, -25.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	//�� ��ü�� �����ϰ� ���� ���Ե� ���� ��ü���� �����Ѵ�.
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	//�� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰��Ѵ�.
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	//�׷��� ��� ����Ʈ���� ��� ����� ������ ��ٸ���.
	WaitForGpuComplete();
	//�׷��� ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵��� �Ҹ��Ų��. 
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
	//CPU �潺�� ���� �����Ѵ�. 

	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
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
	//Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�.
	m_GameTimer.Tick(0.0f);

	ProcessInput();

	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	//��� �Ҵ��ڿ� ��� ����Ʈ�� �����Ѵ�.

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //1�б⶧���� �̰;� ���ҽ�����(��� ��ȭ)�� ��Ÿ���� ���� �庮 
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;   //�̰� ����?
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;  //��������
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //���Ļ���
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; //�츮�� ���ҽ���ü
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ���. ������Ʈ�� ������ ���� Ÿ�� ������ ���´� ������Ʈ ����
	(D3D12_RESOURCE_STATE_PRESENT)���� ���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� ��
	�� ���̴�.*/

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);
	//������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ����Ѵ�. 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//����-���ٽ� �������� CPU �ּҸ� ����Ѵ�.

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	//���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ���-���� �ܰ�(OM)�� �����Ѵ�. //������ �ڵ�� ���⿡ �߰��� ���̴�.

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	//���ϴ� �������� ���� Ÿ��(��)�� �����. 

	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//���ϴ� ������ ����-���ٽ�(��)�� �����. 


	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);  //���׸���



	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���. GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ��
	�� ���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.*/

	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� ���� ���·� �����.

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	//��� ����Ʈ�� ��� ť�� �߰��Ͽ� �����Ѵ�.
	WaitForGpuComplete();
	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���. 

	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;

	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);//�����ϱ� 4���ķ� >Present1(1, 0, &dxgiPresentParameters)����  present(0,0)���� �ٲ�
	/*����ü���� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� ��
	������ �ٲ� ���̴�.*/


	/*������ ������ ����Ʈ�� ���ڿ��� �����ͼ� �� �������� Ÿ��Ʋ�� ����Ѵ�. m_pszBuffer ���ڿ���
"LapProject ("���� �ʱ�ȭ�Ǿ����Ƿ� (m_pszFrameRate+12)�������� ������ ����Ʈ�� ���ڿ��� ���
�Ͽ� �� FPS)�� ���ڿ��� ��ģ��. ::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate+12), 37, 10);
::wcscat_s((m_pszFrameRate+12), 37, _T(" FPS)"));
*/

	MoveToNextFrame(); //��������������!

	m_GameTimer.GetFrameRate(m_pszFrameRate + 14, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate); //������ ������ m_pszFrameRate

}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();
	//GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ���.

	ReleaseObjects();
	//���� ��ü(���� ���� ��ü)�� �Ҹ��Ѵ�. 

	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dRtvDescriptorHeap) //��
		m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dDepthStencilBuffer) //���ٽǹ���
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

//Ǯ��ũ�� �����
void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL); //�Ű���������(���� ����ü���� Ǯ��ũ�� ������� Ȯ��,��� ����ʹ� �ڵ�����)
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters; //Describes a display mode.
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;// �ĸ���� �Ӻ�
	dxgiTargetParameters.Height = m_nWndClientHeight; //�ĸ���� ����
	dxgiTargetParameters.RefreshRate.Numerator = 60; //�и�
	dxgiTargetParameters.RefreshRate.Denominator = 1;//����
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //����Ϳ� �׸��� ��ĳ ���ⲫ�� �ִ� �ɼ�
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters); //���Ÿ���� ũ�⸦ ����

	//m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL); //�����߰��ѳ���
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i]) //�����ϱ� ������ m_ppd3dSwapChainBackBuffers �̰ſ���
			m_ppd3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags); //����ü���� �ĸ� ������ ũ��, ����, ������ ����

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();

}