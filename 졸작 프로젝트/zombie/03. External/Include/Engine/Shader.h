#pragma once
#include "Resource.h"
class CShader :
	public CResource
{
private:
	ComPtr<ID3DBlob>					m_pVSBlob;
	ComPtr<ID3DBlob>					m_pHSBlob;
	ComPtr<ID3DBlob>					m_pDSBlob;
	ComPtr<ID3DBlob>					m_pGSBlob;
	ComPtr<ID3DBlob>					m_pPSBlob;
	ComPtr<ID3DBlob>					m_pErrBlob;
	ComPtr<ID3DBlob>					m_pCSBlob;

	ComPtr<ID3D12PipelineState>			m_pPilelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_tPipeline;

public:
	void Create();
	void CreateVertexShader(const wstring& _strPath, const string& _strFuncName, const string& _strhlslVersion);
	void CreatePixelShader(const wstring& _strPath, const string& _strFuncName, const string& _strhlslVersion);
	//void CreateHullShader();
	//void CreateDomainShader();
	//void CreateGeometryShader();
	//void CreateComputeShader();

	void UpdateData();

public:
	CShader();
	virtual ~CShader();
};

