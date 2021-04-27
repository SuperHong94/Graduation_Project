#include "stdafx.h"
#include "Device.h"

#include "ConstantBuffer.h"
#include "Texture.h"

#include "ResMgr.h"
#include "RenderMgr.h"
#include "MRT.h"

#include "StructuredBuffer.h"

CDevice::CDevice()
	: m_pDevice(nullptr)
	, m_pFence(nullptr)
	, m_pFactory(nullptr)	
	, m_hFenceEvent(nullptr)
	, m_iFenceValue(0)
	, m_iCurDummyIdx(0)
{
	m_vecCB.resize((UINT)CONST_REGISTER::END);
}

CDevice::~CDevice()
{		
	WaitForFenceEvent();
	CloseHandle(m_hFenceEvent);
	
	for (size_t i = 0; i < m_vecCB.size(); ++i)
	{
		SAFE_DELETE(m_vecCB[i]);
	}

	BOOL bIsFullScreen = false;
	m_pSwapChain->GetFullscreenState(&bIsFullScreen, nullptr);
	if (bIsFullScreen)
	{
		m_pSwapChain->SetFullscreenState(false, nullptr);
	}
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
	   	
	IDXGIAdapter* pd3dAdapter = NULL;
	m_pFactory->EnumAdapters(1, &pd3dAdapter);

	// CreateDevice
	D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
	//D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));

	// CreateFence
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFenceCompute));
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
		

	// Compute Command Queue
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCmdQueueCompute));

	// Create Command Allocator
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCmdAlloc));
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCmdAllocRes));
	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pCmdAllocCompute));

	// Create the command list.
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListGraphic));
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCmdAllocRes.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListRes));
	m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pCmdAllocCompute.Get(), nullptr, IID_PPV_ARGS(&m_pCmdListCompute));

	m_pCmdListGraphic->Close();
	m_pCmdListCompute->Close();

	m_pCmdAllocCompute->Reset();
	m_pCmdListCompute->Reset(m_pCmdAllocCompute.Get(), nullptr);

	// SwapChain 만들기
	CreateSwapChain();
			
	// RootSignature 만들기
	CreateRootSignature();

	return S_OK;
}


void CDevice::render_start(float(&_arrFloat)[4])
{
	m_iCurDummyIdx = 0;

	// 그리기 준비	
	m_pCmdAlloc->Reset();
	m_pCmdListGraphic->Reset(m_pCmdAlloc.Get(), nullptr);

	// 필요한 상태 설정	
	// RootSignature 설정	
	CMDLIST->SetGraphicsRootSignature(CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
		
		
	CMRT* pSwapChainMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; 
	barrier.Transition.pResource = pSwapChainMRT->GetRTTex(m_iCurTargetIdx)->GetTex2D().Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// 출력에서
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 다시 백버퍼로 지정
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCmdListGraphic->ResourceBarrier(1, &barrier);
	
	// 첫번째 더미 Descriptor Heap 초기화
	ClearDymmyDescriptorHeap(0);
}

void CDevice::render_present()
{
	CMRT* pSwapChainMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; ;
	barrier.Transition.pResource = pSwapChainMRT->GetRTTex(m_iCurTargetIdx)->GetTex2D().Get();// m_arrRenderTargets[m_iCurTargetIdx]->GetTex2D().Get();
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

	// 상수버퍼 오프셋 초기화
	for (size_t i = 0; i < m_vecCB.size(); ++i)
	{
		m_vecCB[i]->Clear();
	}

	// 백버퍼 타겟 인덱스 변경
	m_iCurTargetIdx == 0 ? m_iCurTargetIdx = 1 : m_iCurTargetIdx = 0;

}

void CDevice::WaitForFenceEvent()
{
	// Signal and increment the fence value.
	const size_t fence = m_iFenceValue;
	m_pCmdQueue->Signal(m_pFence.Get(), fence);
	m_iFenceValue++;
	
	size_t a = m_pFence->GetCompletedValue();
	// Wait until the previous frame is finished.
	if (a < fence)
	{
		m_pFence->SetEventOnCompletion(fence, m_hFenceEvent);
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CDevice::WaitForFenceEvent_CS()
{
	// Signal and increment the fence value.
	static size_t iFenceValue = 1;

	const size_t fence = iFenceValue;
	m_pCmdQueueCompute->Signal(m_pFenceCompute.Get(), fence);
	iFenceValue++;

	size_t a = m_pFenceCompute->GetCompletedValue();
	// Wait until the previous frame is finished.
	if (a < fence)
	{
		m_pFenceCompute->SetEventOnCompletion(fence, m_hFenceEvent);
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
	tDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//tDesc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	
	tDesc.OutputWindow = m_hWnd;	// 출력 윈도우
	tDesc.Windowed = m_bWindowed;   // 창 모드 or 전체화면 모드
	tDesc.SampleDesc.Count = 1;		// 멀티 샘플 사용 안함
	tDesc.SampleDesc.Quality = 0;
	tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
		
	HRESULT hr = m_pFactory->CreateSwapChain(m_pCmdQueue.Get(), &tDesc, &m_pSwapChain);

	m_pSwapChain->GetContainingOutput(&dxgiOutput);
	if (dxgiOutput)
	{

	}
}

void CDevice::CreateRootSignature()
{
	m_iCBVIncreSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 슬롯 별 Descriptor Table 을 작성한다.
	// 0 번 슬롯
	vector< D3D12_DESCRIPTOR_RANGE> vecRange;

	D3D12_ROOT_PARAMETER slotParam = {};
	vecRange.clear();
	
	D3D12_DESCRIPTOR_RANGE range = {};
			
	range.BaseShaderRegister = 0;  // b0 에서
	range.NumDescriptors = (UINT)CONST_REGISTER::END;	  // b5 까지 6개 상수레지스터 사용여부 
	range.OffsetInDescriptorsFromTableStart = -1;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	vecRange.push_back(range);

	range = {};
	range.BaseShaderRegister = 0;  // t0 에서
	range.NumDescriptors = 13;	   // t12 까지 13 개 텍스쳐 레지스터 사용여부 
	range.OffsetInDescriptorsFromTableStart = (UINT)CONST_REGISTER::END;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	vecRange.push_back(range);

	slotParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	slotParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	slotParam.DescriptorTable.NumDescriptorRanges = (UINT)vecRange.size();
	slotParam.DescriptorTable.pDescriptorRanges = &vecRange[0];
			
	// Sampler Desc 만들기
	CreateSamplerDesc();

	// 루트 서명 	
	D3D12_ROOT_SIGNATURE_DESC sigDesc = {};
	sigDesc.NumParameters = 1;
	sigDesc.pParameters = &slotParam;
	sigDesc.NumStaticSamplers = 2;// m_vecSamplerDesc.size();
	sigDesc.pStaticSamplers = &m_vecSamplerDesc[0]; // 사용 될 Sampler 정보
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계 허용
	
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	HRESULT hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_arrSig[(UINT)ROOT_SIG_TYPE::RENDER]));	

	// 더미용 Descriptor Heap 만들기
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

	UINT iDescriptorNum = 0;
	for (size_t i = 0; i < vecRange.size(); ++i)
	{
		iDescriptorNum += vecRange[i].NumDescriptors;
	}

	cbvHeapDesc.NumDescriptors = iDescriptorNum;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	for (size_t i = 0; i < 512; ++i)
	{
		ComPtr<ID3D12DescriptorHeap> pDummyDescriptor;
		DEVICE->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&pDummyDescriptor));
		m_vecDummyDescriptor.push_back(pDummyDescriptor);
	}

		
	// ====================================
	// Compute Shader 전용 Signature 만들기
	// ====================================
	range = {};
	range.BaseShaderRegister = 0;  // u0 에서
	range.NumDescriptors = 4;	   // u3 까지 4 개 UAV 레지스터 사용여부 
	range.OffsetInDescriptorsFromTableStart = (UINT)TEXTURE_REGISTER::END;
	range.RegisterSpace = 0;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	
	vecRange.push_back(range);
	
	slotParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	slotParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	slotParam.DescriptorTable.NumDescriptorRanges = (UINT)vecRange.size();
	slotParam.DescriptorTable.pDescriptorRanges = &vecRange[0];	

	sigDesc = {};
	sigDesc.NumParameters = 1;
	sigDesc.pParameters = &slotParam;
	sigDesc.NumStaticSamplers = 0;
	sigDesc.pStaticSamplers = nullptr; // 컴퓨트 쉐이더에서는 Sampler 사용 불가능
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE; // 컴퓨트 쉐이더용 Signature 는 기본 플래그 값으로 설정

	pSignature = nullptr;
	pError = nullptr;

	hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
	m_pDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize()
								  , IID_PPV_ARGS(&m_arrSig[(UINT)ROOT_SIG_TYPE::COMPUTE]));

	// 생성된 컴퓨트 쉐이더용 루트시그네이쳐를 컴퓨트 쉐이더 커맨드 리스트에 등록
	CMDLIST_CS->SetComputeRootSignature(CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::COMPUTE).Get());

	// ComputeShader 더미 디스크립터 힙 작성	
	cbvHeapDesc.NumDescriptors += range.NumDescriptors; // UAV 만큼 추가
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DEVICE->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_pDummyDescriptorCompute));

	// 초기화용 더미 디스크립터 힙 작성	
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DEVICE->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_pInitDescriptor));
}

void CDevice::CreateSamplerDesc()
{
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_ANISOTROPIC;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 4;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = 0.f;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	m_vecSamplerDesc.push_back(sampler);

	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 1;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	m_vecSamplerDesc.push_back(sampler);
}

void CDevice::CreateConstBuffer(const wstring & _strName, size_t _iSize
	, size_t _iMaxCount, CONST_REGISTER _eRegisterNum, bool _bGlobal)
{	
	CConstantBuffer* pCB = new CConstantBuffer;
	pCB->SetName(_strName);
	pCB->Create((UINT)_iSize, (UINT)_iMaxCount, _eRegisterNum);
	m_vecCB[(UINT)_eRegisterNum] = pCB;

	if (_bGlobal)
	{
		SetGlobalConstBufferToRegister(pCB, 0);
	}
}

void CDevice::SetConstBufferToRegister(CConstantBuffer * _pCB, UINT _iOffset)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_pCB->GetRegisterNum();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pCB->GetCBV()->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += _iOffset * m_iCBVIncreSize;

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetGlobalConstBufferToRegister(CConstantBuffer * _pCB, UINT _iOffset)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_pCB->GetRegisterNum();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pCB->GetCBV()->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += _iOffset * m_iCBVIncreSize;

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetTextureToRegister(CTexture * _pTex, TEXTURE_REGISTER _eRegisterNum)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pTex->GetSRV()->GetCPUDescriptorHandleForHeapStart();	

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pTex->GetTex2D().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pTex->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);
		_pTex->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}

}

void CDevice::SetBufferToRegister(CStructuredBuffer * _pBuffer, TEXTURE_REGISTER _eRegisterNum)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[m_iCurDummyIdx]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegisterNum;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pBuffer->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pBuffer->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pBuffer->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);

		_pBuffer->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::ClearDymmyDescriptorHeap(UINT _iDummyIndex)
{	
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_vecDummyDescriptor[_iDummyIndex]->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr;

	UINT iDestRange = (UINT)TEXTURE_REGISTER::END;
	UINT iSrcRange = (UINT)TEXTURE_REGISTER::END;

	m_pDevice->CopyDescriptors(1/*디스크립터 개수*/
						     , &hDescHandle, &iDestRange
						     , 1/*디스크립터 개수*/
						     , &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetConstBufferToRegister_CS(CConstantBuffer * _pCB, UINT _iOffset)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_pCB->GetRegisterNum();

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pCB->GetCBV()->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr += _iOffset * m_iCBVIncreSize;

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::SetTextureToRegister_CS(CTexture * _pTex, TEXTURE_REGISTER _eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pTex->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pTex->GetTex2D().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pTex->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);
		_pTex->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::SetUAVToRegister_CS(CTexture * _pTex, UAV_REGISTER _eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pTex->GetUAV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pTex->GetTex2D().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pTex->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);
		_pTex->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::SetBufferToSRVRegister_CS(CStructuredBuffer * _pBuffer, TEXTURE_REGISTER _eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pBuffer->GetSRV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pBuffer->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pBuffer->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);

		_pBuffer->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::SetBufferToUAVRegister_CS(CStructuredBuffer * _pBuffer, UAV_REGISTER _eRegister)
{
	UINT iDestRange = 1;
	UINT iSrcRange = 1;

	// 0번 슬롯이 상수버퍼 데이터
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr += m_iCBVIncreSize * (UINT)_eRegister;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = _pBuffer->GetUAV()->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CopyDescriptors(1, &hDescHandle, &iDestRange
		, 1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 리소스 상태 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_pBuffer->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (_pBuffer->GetResState() == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		CMDLIST_CS->ResourceBarrier(1, &resurceBarrier);

		_pBuffer->SetResState(D3D12_RESOURCE_STATE_COMMON);
	}
}

void CDevice::ClearDymmyDescriptorHeap_CS()
{
	D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pDummyDescriptorCompute->GetCPUDescriptorHandleForHeapStart();
	hDescHandle.ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pInitDescriptor->GetCPUDescriptorHandleForHeapStart();
	hSrcHandle.ptr;

	UINT iDestRange = (UINT)UAV_REGISTER::END;
	UINT iSrcRange = (UINT)UAV_REGISTER::END;

	m_pDevice->CopyDescriptors(1/*디스크립터 개수*/
		, &hDescHandle, &iDestRange
		, 1/*디스크립터 개수*/
		, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CDevice::UpdateTable()
{
	ID3D12DescriptorHeap* pDescriptor = m_vecDummyDescriptor[m_iCurDummyIdx].Get();
	m_pCmdListGraphic->SetDescriptorHeaps(1, &pDescriptor);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = pDescriptor->GetGPUDescriptorHandleForHeapStart();
	m_pCmdListGraphic->SetGraphicsRootDescriptorTable(0, gpuhandle);

	// 다음 더미 Descriptor Heap 을 가리키게 인덱스를 증가시킨다.
	++m_iCurDummyIdx;

	// 다음 더비 Descriptor Heap 을 초기화 한다.(전역 상수버퍼는 남는다)
	ClearDymmyDescriptorHeap(m_iCurDummyIdx);	
}

void CDevice::UpdateTable_CS()
{
	ID3D12DescriptorHeap* pDescriptor = m_pDummyDescriptorCompute.Get();
	m_pCmdListCompute->SetDescriptorHeaps(1, &pDescriptor);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = pDescriptor->GetGPUDescriptorHandleForHeapStart();
	m_pCmdListCompute->SetComputeRootDescriptorTable(0, gpuhandle);
}

void CDevice::ExcuteResourceLoad()
{
	// 리소스 로딩 명령 닫기
	m_pCmdListRes->Close();

	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListRes.Get() };
	m_pCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	
	WaitForFenceEvent();

	// 다시 활성화
	m_pCmdAllocRes->Reset();
	m_pCmdListRes->Reset(m_pCmdAllocRes.Get(), nullptr);
}

void CDevice::ExcuteComputeShader()
{
	// 컴퓨트 쉐이더 명령 닫기
	m_pCmdListCompute->Close();

	// 커맨드 리스트 수행	
	ID3D12CommandList* ppCommandLists[] = { m_pCmdListCompute.Get() };
	m_pCmdQueueCompute->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitForFenceEvent_CS();

	// 다시 활성화
	m_pCmdAllocCompute->Reset();
	m_pCmdListCompute->Reset(m_pCmdAllocCompute.Get(), nullptr);

	// 루트서명 등록
	m_pCmdListCompute->SetComputeRootSignature(CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::COMPUTE).Get());
}
