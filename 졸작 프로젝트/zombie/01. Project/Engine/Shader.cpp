#include "stdafx.h"
#include "Shader.h"

#include "Device.h"
#include "PathMgr.h"


CShader::CShader()
	: CResource(RES_TYPE::SHADER)
	, m_pVSBlob(nullptr)
	, m_pHSBlob(nullptr)
	, m_pDSBlob(nullptr)
	, m_pGSBlob(nullptr)
	, m_pPSBlob(nullptr)
	, m_pErrBlob(nullptr)
	, m_pCSBlob(nullptr)
	, m_pPilelineState(nullptr)
	, m_tPipeline {}
{
}

CShader::~CShader()
{	
}

void CShader::Create()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_tPipeline.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	m_tPipeline.pRootSignature = CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get();
		
	m_tPipeline.RasterizerState  = g_arrRSDesc[(UINT)RS_TYPE::DEFAULT];
	m_tPipeline.BlendState		 = g_arrBlendDesc[(UINT)BLEND_TYPE::DEFAULT];
	m_tPipeline.DepthStencilState.DepthEnable = FALSE;
	m_tPipeline.DepthStencilState.StencilEnable = FALSE;
	m_tPipeline.SampleMask = UINT_MAX;
	m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_tPipeline.NumRenderTargets = 1;
	m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_tPipeline.SampleDesc.Count = 1;

	DEVICE->CreateGraphicsPipelineState(&m_tPipeline, IID_PPV_ARGS(&m_pPilelineState));
}

void CShader::CreateVertexShader(const wstring & _strPath, const string & _strFuncName, const string & _strhlslVersion)
{
	int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;	
	
	char* pErr = nullptr;

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), _strhlslVersion.c_str(), iFlag, 0, &m_pVSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Create Failed !!!", MB_OK);
	}

	m_tPipeline.VS = { m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize() };
}


void CShader::CreatePixelShader(const wstring & _strPath, const string & _strFuncName, const string & _strhlslVersion)
{
	int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;

	char* pErr = nullptr;

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), _strhlslVersion.c_str(), iFlag, 0, &m_pPSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Create Failed !!!", MB_OK);
	}

	m_tPipeline.PS = { m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize() };
}

void CShader::UpdateData()
{	
	CMDLIST->SetPipelineState(m_pPilelineState.Get());	
}