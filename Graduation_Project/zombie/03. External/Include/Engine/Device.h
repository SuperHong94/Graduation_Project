#pragma once

class CConstantBuffer;

#include "Ptr.h"
#include "Texture.h"

class CStructuredBuffer;

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

	// 컴퓨트 쉐이더 커맨드 큐
	ComPtr<ID3D12CommandQueue>				m_pCmdQueueCompute;

	// 렌더링 커맨드 리스트
	ComPtr<ID3D12CommandAllocator>			m_pCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>		m_pCmdListGraphic;

	// 리소스 로딩 커맨드 리스트
	ComPtr<ID3D12CommandAllocator>			m_pCmdAllocRes;	
	ComPtr<ID3D12GraphicsCommandList>		m_pCmdListRes;	

	// 컴퓨터 쉐이더 커맨드 리스트
	ComPtr<ID3D12CommandAllocator>			m_pCmdAllocCompute;
	ComPtr<ID3D12GraphicsCommandList>		m_pCmdListCompute;


	ComPtr<ID3D12Fence>						m_pFence;
	ComPtr<ID3D12Fence>						m_pFenceCompute;

	ComPtr<IDXGIFactory>					m_pFactory;
	ComPtr<ID3D12Debug>						m_pDbgCtrl;		// 디버그 관리	

	ComPtr<IDXGISwapChain>					m_pSwapChain;	
	//Ptr<CTexture>							m_arrRenderTargets[2];
	//Ptr<CTexture>							m_pDSTex;

	vector<ComPtr<ID3D12DescriptorHeap>>	m_vecDummyDescriptor;
	ComPtr<ID3D12DescriptorHeap>			m_pDummyDescriptorCompute;
	UINT									m_iCurDummyIdx;
	ComPtr<ID3D12DescriptorHeap>			m_pInitDescriptor;	
	UINT									m_iCBVIncreSize; // 핸들 오프셋 증감 사이즈

	vector<D3D12_STATIC_SAMPLER_DESC>		m_vecSamplerDesc;
	D3D12_VIEWPORT							m_tVP;
	D3D12_RECT								m_tScissorRect;	

	HANDLE									m_hFenceEvent;
	size_t									m_iFenceValue;
	UINT									m_iCurTargetIdx;	

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
	void WaitForFenceEvent_CS();

	void SetConstBufferToRegister(CConstantBuffer* _pCB, UINT _iOffset);	
	void SetGlobalConstBufferToRegister(CConstantBuffer* _pCB, UINT _iOffset);
	void SetTextureToRegister(CTexture* _pTex, TEXTURE_REGISTER _eRegister);
	void SetBufferToRegister(CStructuredBuffer* _pBuffer, TEXTURE_REGISTER _eRegister);
	void ClearDymmyDescriptorHeap(UINT _iDummyIndex);

	void SetConstBufferToRegister_CS(CConstantBuffer* _pCB, UINT _iOffset);	
	void SetTextureToRegister_CS(CTexture* _pTex, TEXTURE_REGISTER _eRegister);
	void SetUAVToRegister_CS(CTexture* _pTex, UAV_REGISTER _eRegister);
	void SetBufferToSRVRegister_CS(CStructuredBuffer* _pBuffer, TEXTURE_REGISTER _eRegister);
	void SetBufferToUAVRegister_CS(CStructuredBuffer* _pBuffer, UAV_REGISTER _eRegister);
	void ClearDymmyDescriptorHeap_CS();

	void UpdateTable();
	void UpdateTable_CS();
	void ExcuteResourceLoad();
	void ExcuteComputeShader();

private:
	void CreateSwapChain();	
	void CreateViewPort();	
	void CreateRootSignature();
	void CreateSamplerDesc(); 
	

public:
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return m_pCmdListGraphic; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdListRes() { return m_pCmdListRes; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdListCompute() { return m_pCmdListCompute; }
	ComPtr<ID3D12Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D12RootSignature> GetRootSignature(ROOT_SIG_TYPE _eType) { return m_arrSig[(UINT)_eType]; }
	CConstantBuffer* GetCB(CONST_REGISTER _eRegister) { return m_vecCB[(UINT)_eRegister]; }	
	UINT GetSwapchainIdx() { return m_iCurTargetIdx; }
	ComPtr<IDXGISwapChain> GetSwapChain() { return m_pSwapChain; }
};

