#pragma once

class CConstantBuffer;
class CTexture;

class CDevice
{
	SINGLE(CDevice);
public:
	HWND									m_hWnd;
	tResolution								m_tResolution;
	bool									m_bWindowed;

private:
	ComPtr<ID3D12Device>					m_pDevice;		// 메모리 할당, 객체 생성	
	ComPtr<ID3D12CommandQueue>				m_pCmdQueue;	

	// 렌더링 커맨드 리스트
	ComPtr<ID3D12CommandAllocator>			m_pCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>		m_pCmdListGraphic;

	// 리소스 로딩 리스트
	ComPtr<ID3D12CommandAllocator>			m_pCmdAllocRes;	
	ComPtr<ID3D12GraphicsCommandList>		m_pCmdListRes;	

	ComPtr<ID3D12Fence>						m_pFence;
	ComPtr<IDXGIFactory>					m_pFactory;
	ComPtr<ID3D12Debug>						m_pDbgCtrl;		// 디버그 관리	

	ComPtr<IDXGISwapChain>					m_pSwapChain;
	ComPtr<ID3D12Resource>					m_RenderTargets[2];
	ComPtr<ID3D12DescriptorHeap>			m_pRTV;

	ComPtr<ID3D12Resource>					m_pDepthStencilTex;
	ComPtr<ID3D12DescriptorHeap>			m_pDSV;

	vector<ComPtr<ID3D12DescriptorHeap>>	m_vecDummyDescriptor;
	UINT									m_iCurDummyIdx;
	ComPtr<ID3D12DescriptorHeap>			m_pInitDescriptor;
	UINT									m_iCBVIncreSize; // 핸들 오프셋 증감 사이즈

	vector<D3D12_STATIC_SAMPLER_DESC>		m_vecSamplerDesc;
	D3D12_VIEWPORT							m_tVP;
	D3D12_RECT								m_tScissorRect;	

	HANDLE									m_hFenceEvent;
	size_t									m_iFenceValue;
	UINT									m_iCurTargetIdx;
	size_t									m_iRTVHeapSize;

	vector<CConstantBuffer*>				m_vecCB;


private:
	ComPtr<ID3D12RootSignature>				m_arrSig[(UINT)ROOT_SIG_TYPE::END];

public:
	int init(HWND _hWnd, const tResolution& _res, bool _bWindow);

	void CreateConstBuffer(const wstring& _strName, size_t _iBufferSize
		, size_t _iMaxCount, CONST_REGISTER _eRegisterNum
		, bool _bGlobal = false);

	void render_start(float(&_arrFloat)[4]); 	

	void render_present();
	void WaitForFenceEvent();

	void SetConstBufferToRegister(CConstantBuffer* _pCB, UINT _iOffset);	
	void SetGlobalConstBufferToRegister(CConstantBuffer* _pCB, UINT _iOffset);
	void SetTextureToRegister(CTexture* _pTex, TEXTURE_REGISTER _eRegister);
	void ClearDymmyDescriptorHeap(UINT _iDummyIndex);

	void UpdateTable();
	void ExcuteResourceLoad();

private:
	void CreateSwapChain();
	void CreateView();
	void CreateViewPort();	
	void CreateRootSignature();
	void CreateSamplerDesc(); 
	

public:
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return m_pCmdListGraphic; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdListRes() { return m_pCmdListRes; }
	ComPtr<ID3D12Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D12RootSignature> GetRootSignature(ROOT_SIG_TYPE _eType) { return m_arrSig[(UINT)_eType]; }
	CConstantBuffer* GetCB(CONST_REGISTER _eRegister) { return m_vecCB[(UINT)_eRegister]; }
};

