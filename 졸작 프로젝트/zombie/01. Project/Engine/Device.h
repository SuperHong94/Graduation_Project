#pragma once


class CDevice
{
	SINGLE(CDevice);
public:
	HWND							m_hWnd;
	tResolution						m_tResolution;
	bool							m_bWindowed;

private:
	ComPtr<ID3D12Device>				m_pDevice;		// 메모리 할당, 객체 생성
	ComPtr<ID3D12PipelineState>			m_pPipelineState;
	ComPtr<ID3D12CommandQueue>			m_pCmdQueue;	
	ComPtr<ID3D12CommandAllocator>		m_pCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	m_pCmdListGraphic;
	ComPtr<ID3D12Fence>					m_pFence;
	ComPtr<IDXGIFactory>				m_pFactory;
	ComPtr<ID3D12Debug>					m_pDbgCtrl;		// 디버그 관리	

	ComPtr<IDXGISwapChain>			m_pSwapChain;
	ComPtr<ID3D12Resource>			m_RenderTargets[2];
	ComPtr<ID3D12DescriptorHeap>	m_pRTV;
	ComPtr<ID3D12DescriptorHeap>	m_pDSV;

	D3D12_VIEWPORT					m_tVP;
	D3D12_RECT						m_tScissorRect;	

	HANDLE							m_hFenceEvent;
	size_t							m_iFenceValue;
	UINT							m_iCurTargetIdx;
	size_t							m_iRTVHeapSize;

private:
	ComPtr<ID3D12RootSignature>	m_arrSig[(UINT)ROOT_SIG_TYPE::END];

public:
	int init(HWND _hWnd, const tResolution& _res, bool _bWindow);
	void render_start(float(&_arrFloat)[4]); 	

	void render_present();
	void WaitForFenceEvent();
	
private:
	void CreateSwapChain();
	void CreateView();
	void CreateViewPort();

public:
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return m_pCmdListGraphic; }
	ComPtr<ID3D12Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D12RootSignature> GetRootSignature(ROOT_SIG_TYPE _eType) { return m_arrSig[(UINT)_eType]; }
};

