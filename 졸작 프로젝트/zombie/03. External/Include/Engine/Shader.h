#pragma once
#include "Resource.h"


struct tShaderParam
{
	wstring			strParamName;
	SHADER_PARAM	eType;
};



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

	D3D_PRIMITIVE_TOPOLOGY				m_eTopology;
	vector<tShaderParam>				m_vecShaderPamam;

	RS_TYPE								m_eRSType;
	BLEND_TYPE							m_eBlendType;
	DEPTH_STENCIL_TYPE					m_eDSType;
	

public:
	void Create(D3D_PRIMITIVE_TOPOLOGY _eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void CreateVertexShader(const wstring& _strPath, const string& _strFuncName, const string& _strhlslVersion);
	void CreatePixelShader(const wstring& _strPath, const string& _strFuncName, const string& _strhlslVersion);
	//void CreateHullShader();
	//void CreateDomainShader();
	//void CreateGeometryShader();
	//void CreateComputeShader();

	void SetBlendState(BLEND_TYPE _eType) { m_eBlendType = _eType; }
	void SetDepthStencilType(DEPTH_STENCIL_TYPE _eType);
	void SetRasterizerType(RS_TYPE _eType) { m_eRSType = _eType; }

	void UpdateData();
	void AddShaderParam(const tShaderParam& _param) { m_vecShaderPamam.push_back(_param); }
public:
	CShader();
	virtual ~CShader();
};

