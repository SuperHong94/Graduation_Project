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
	, m_pPipelineState(nullptr)
	, m_eBlendType(BLEND_TYPE::DEFAULT)
	, m_eDSType(DEPTH_STENCIL_TYPE::LESS)
	, m_tPipeline {}
	, m_tCSStateDesc{}
{
}

CShader::~CShader()
{	
}

void CShader::SetDepthStencilType(DEPTH_STENCIL_TYPE _eType)
{
	m_eDSType = _eType;	
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


void CShader::CreateGeometryShader(const wstring & _strPath, const string & _strFuncName, const string & _strhlslVersion)
{
	int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;

	char* pErr = nullptr;

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), _strhlslVersion.c_str(), iFlag, 0, &m_pGSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Create Failed !!!", MB_OK);
	}

	m_tPipeline.GS = { m_pGSBlob->GetBufferPointer(), m_pGSBlob->GetBufferSize() };
}

void CShader::CreateComputeShader(const wstring & _strPath, const string & _strFuncName, const string & _strhlslVersion)
{
	int iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;

	char* pErr = nullptr;

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), _strhlslVersion.c_str(), iFlag, 0, &m_pCSBlob, &m_pErrBlob)))
	{
		pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Create Failed !!!", MB_OK);
	}

	m_tCSStateDesc.pRootSignature = CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::COMPUTE).Get();
	m_tCSStateDesc.CS = { m_pCSBlob->GetBufferPointer(), m_pCSBlob->GetBufferSize() };

	DEVICE->CreateComputePipelineState(&m_tCSStateDesc, IID_PPV_ARGS(&m_pPilelineState_CS));

	m_ePOV = SHADER_POV::COMPUTE;
}

void CShader::UpdateData()
{		
	CMDLIST->SetPipelineState(m_pPipelineState.Get());
	CMDLIST->IASetPrimitiveTopology(m_eTopology);	
}

void CShader::UpdateData_CS()
{
	CMDLIST_CS->SetPipelineState(m_pPilelineState_CS.Get());	
}

void CShader::Create(SHADER_POV _ePOV, D3D_PRIMITIVE_TOPOLOGY _eTopology)
{
	m_eTopology = _eTopology;
	m_ePOV = _ePOV;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	m_tPipeline.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	m_tPipeline.pRootSignature = CDevice::GetInst()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get();

	m_tPipeline.RasterizerState = g_arrRSDesc[(UINT)m_eRSType];
	m_tPipeline.BlendState = g_arrBlendDesc[(UINT)m_eBlendType];
	
	m_tPipeline.DepthStencilState = g_arrDepthStencilDesc[(UINT)m_eDSType];

	m_tPipeline.SampleMask = UINT_MAX;	
	m_tPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	m_tPipeline.SampleDesc.Count = 1;


	switch (m_ePOV)
	{
	case SHADER_POV::DEFERRED:
		m_tPipeline.NumRenderTargets = 3;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_tPipeline.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case SHADER_POV::FORWARD:
	case SHADER_POV::POST_EFFECT:
	case SHADER_POV::PARTICLE:
		m_tPipeline.NumRenderTargets = 1;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	
		break;
	case SHADER_POV::LIGHTING:
		m_tPipeline.NumRenderTargets = 2;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_tPipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_POV::COMPUTE:
		m_tPipeline.NumRenderTargets = 0;
		break;
	default:
		m_tPipeline.NumRenderTargets = 1;
		m_tPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	switch (_eTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		m_tPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		break;
	default:
		assert(nullptr);
	}

	HRESULT hr = DEVICE->CreateGraphicsPipelineState(&m_tPipeline, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr))
		assert(nullptr);
}
