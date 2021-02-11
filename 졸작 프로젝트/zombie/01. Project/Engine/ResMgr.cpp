#include "stdafx.h"
#include "ResMgr.h"

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Sound.h"

//#include "RenderMgr.h"


CResMgr::CResMgr()
{
}

CResMgr::~CResMgr()
{
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Map(m_mapRes[i]);
	}

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Delete_Vector(m_vecCloneRes[i]);
	}
}

void CResMgr::init()
{
	CreateDefaultMesh();

	CreateDefaultShader();

	CreateDefaultMaterial();

	// FMOD 초기화
	InitSound();
}

//Ptr<CTexture> CResMgr::CreateTexture(const wstring _strKey, float _fWidth, float _iHeight
//	, UINT _iBindFlag, DXGI_FORMAT _eFormat, D3D11_USAGE _eUsage)
//{
//	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
//
//	if (nullptr != pTex)
//		assert(nullptr);
//
//	pTex = new CTexture;	
//	if (!pTex->Create(_fWidth, _iHeight, _iBindFlag, _eFormat, _eUsage))
//	{
//		MessageBox(nullptr, _strKey.c_str(), L"텍스쳐 생성 실패", MB_OK);
//		assert(nullptr);
//	}
//
//	pTex->SetName(_strKey);
//
//	m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.GetPointer()));
//
//	return pTex;
//}
//
//Ptr<CTexture> CResMgr::CreateTexture(const wstring & _strKey, ID3D11Texture2D * _pTex2D)
//{
//	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
//
//	if (nullptr != pTex)
//		assert(nullptr);
//
//	pTex = new CTexture;
//	if (!pTex->Create(_pTex2D))
//	{
//		MessageBox(nullptr, _strKey.c_str(), L"텍스쳐 생성 실패", MB_OK);
//		assert(nullptr);
//	}
//
//	pTex->SetName(_strKey);
//
//	m_mapRes[(UINT)RES_TYPE::TEXTURE].insert(make_pair(_strKey, pTex.GetPointer()));
//
//	return pTex;
//}

void CResMgr::CreateDefaultMesh()
{
	vector<VTX> vecVTX;
	vector<UINT> vecIdx;

	Ptr<CMesh> pMesh = nullptr;
	
	
	// =============	// 0 --- 1
	// Rect Mesh		// |  \  |
	//					// 3 --- 2
	// =============	
	pMesh = new CMesh;

	VTX v;
	// 1. 입력 조립기 단계에 전달할, 정점 3개로 구성된 삼각형 1개
	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);	
	vecVTX.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVTX.push_back(v);

	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(0); vecIdx.push_back(2); vecIdx.push_back(3);

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data());

	AddRes(L"RectMesh", pMesh);	
	
	// =============		// 0 --- 1
	// Collider Rect Mesh	// |     |
	//						// 3 --- 2
	// =============	
	pMesh = new CMesh;
	
	vecIdx.clear();
	vecIdx.push_back(0); vecIdx.push_back(1); 
	vecIdx.push_back(2); vecIdx.push_back(3); vecIdx.push_back(0);

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data()); //D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	
	AddRes(L"ColRectMesh", pMesh);

	//				       3
	// ===========	//    /
	// Dir Mesh		//0,2,4 ㅡ ㅡ 5
	//				//    \
	// ===========	//     1
	pMesh = new CMesh;

	vecVTX.clear();
	vecIdx.clear();

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(1.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	vecVTX.push_back(v);

	v.vPos = Vec3(0.f, 0.f, 1.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	vecVTX.push_back(v);

	vecIdx.push_back(0); vecIdx.push_back(1);
	vecIdx.push_back(2); vecIdx.push_back(3);
	vecIdx.push_back(4); vecIdx.push_back(5);

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data()); // D3D11_PRIMITIVE_TOPOLOGY_LINELIST

	AddRes(L"DirMesh", pMesh);
	   
	// ============
	// Circle Mesh
	// ============
	vecVTX.clear();
	vecIdx.clear();

	pMesh = new CMesh;

	// 원의 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);

	vecVTX.push_back(v);

	UINT iSliceCount = 32;
	float fRadius = 0.5f;
	float fSliceTheta = XM_2PI / iSliceCount;
	
	float fCurTheta = 0.f;
	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{		
		v.vPos = Vec3(fRadius * cosf(fCurTheta), fRadius * sinf(fCurTheta), 0.f);		
		v.vColor = Vec4(1.f, 0.2f, 0.2f, 1.f);
		v.vUV = Vec2(0.5f * cosf(fCurTheta), 0.5f * sinf(fCurTheta));

		v.vUV.x += 0.5f;
		v.vUV.y = (0.5f - v.vUV.y);

		fCurTheta += fSliceTheta;

		vecVTX.push_back(v);
	}
	
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}	

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data());

	AddRes(L"CircleMesh", pMesh);

	// ============
	// ColCircle Mesh
	// ============	
	vecIdx.clear();

	pMesh = new CMesh;

	for (UINT i = 1; i < iSliceCount + 2; ++i)
	{
		vecIdx.push_back(i);
	}

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data()); // D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP

	AddRes(L"ColCircleMesh", pMesh);

	vecVTX.clear();
	vecIdx.clear();

	// =========
	// Cube Mesh
	// =========
	VTX arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh;

	pMesh->Create(sizeof(VTX), 24, (BYTE*)arrCube
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data());

	pMesh->SetName(L"CubeMesh");
	AddRes<CMesh>(pMesh->GetName(), pMesh);	

	vecVTX.clear();
	vecIdx.clear();

	// ============
	// Sphere Mesh
	// ============
	fRadius = 1.f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVTX.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	iSliceCount = 40; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vTangent.Cross(v.vNormal, v.vBinormal);
			v.vBinormal.Normalize();

			vecVTX.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVTX.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVTX.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh;

	pMesh->Create(sizeof(VTX), (UINT)vecVTX.size(), (BYTE*)vecVTX.data()
		, DXGI_FORMAT_R32_UINT, (UINT)vecIdx.size(), (BYTE*)vecIdx.data());

	pMesh->SetName(L"SphereMesh");
	AddRes<CMesh>(pMesh->GetName(), pMesh);

	vecVTX.clear();
	vecIdx.clear();
}

void CResMgr::CreateDefaultShader()
{
	//=============
	// TestShader
	// int 1 개 필요
	//=============
	Ptr<CShader> pShader = nullptr;
	   
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Test", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Test", "ps_5_0");

	// BlendState 설정
	pShader->SetBlendState(BLEND_TYPE::ALPHABLEND);

	pShader->Create();	

	pShader->AddShaderParam(tShaderParam{ L"Test Value", SHADER_PARAM::INT_0 });

	AddRes(L"TestShader", pShader);

	// ==============
	// Texture Shader
	// ==============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Tex", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Tex", "ps_5_0");

	// BlendState 설정
	pShader->SetBlendState(BLEND_TYPE::ALPHABLEND);

	// Shader Parameter 알림
	pShader->AddShaderParam(tShaderParam{ L"Output Texture", SHADER_PARAM::TEX_0 });

	pShader->Create();

	AddRes(L"TexShader", pShader);

	// =================
	// Collider2D Shader
	// =================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Collider2D", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Collider2D", "ps_5_0");

	// DepthStencilState 설정
	pShader->SetDepthStencilType(DEPTH_STENCIL_TYPE::NO_DEPTHTEST);

	pShader->Create(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	AddRes(L"Collider2DShader", pShader);

	// =================
	// STD2D Shader
	// =================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Std2D", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Std2D", "ps_5_0");

	// BlendState 설정
	pShader->SetBlendState(BLEND_TYPE::ALPHABLEND);

	// Parameter 설정
	pShader->AddShaderParam(tShaderParam{ L"Output Texture", SHADER_PARAM::TEX_0});	
	pShader->AddShaderParam(tShaderParam{ L"Anim Tex", SHADER_PARAM::TEX_3});

	pShader->Create();
	AddRes(L"Std2DShader", pShader);

	// =================
	// 2DShadow Shader
	// =================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_2DShadow", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_2DShadow", "ps_5_0");

	// BlendState 설정
	pShader->SetBlendState(BLEND_TYPE::ALPHABLEND);

	// Parameter 설정
	pShader->AddShaderParam(tShaderParam{ L"Output Texture", SHADER_PARAM::TEX_0 });
	pShader->Create();
	AddRes(L"2DShadowShader", pShader);

	// ============
	// Std3D Shader
	// ============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Std3D", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Std3D", "ps_5_0");
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->Create();
	AddRes(L"Std3DShader", pShader);

	// ============
	// Skybox Shader
	// ============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Skybox", "vs_5_0");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Skybox", "ps_5_0");
	pShader->SetRasterizerType(RS_TYPE::CULL_FRONT);
	pShader->Create();
	AddRes(L"SkyboxShader", pShader);
}

void CResMgr::CreateDefaultMaterial()
{
	Ptr<CMaterial> pMtrl = nullptr;
	
	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"TestShader"));
	AddRes(L"TestMtrl", pMtrl);

	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"TexShader"));
	AddRes(L"TexMtrl", pMtrl);

	int a = 0;
	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"Collider2DShader"));
	pMtrl->SetData(SHADER_PARAM::INT_0, &a);
	AddRes(L"Collider2DMtrl_0", pMtrl);

	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"Collider2DShader"));
	a = 1;
	pMtrl->SetData(SHADER_PARAM::INT_0, &a);
	AddRes(L"Collider2DMtrl_1", pMtrl);

	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"Std2DShader"));
	AddRes(L"Std2DMtrl", pMtrl);

	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"DirShader"));
	AddRes(L"DirMtrl", pMtrl);

	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"Std3DShader"));
	AddRes(L"Std3DMtrl", pMtrl);
		
	pMtrl = new CMaterial;
	pMtrl->DisableFileSave();
	pMtrl->SetShader(FindRes<CShader>(L"SkyboxShader"));
	AddRes(L"SkyboxMtrl", pMtrl);

	

	//pMtrl = new CMaterial;
	////pMtrl->DisableFileSave();
	//pMtrl->SetShader(FindRes<CShader>(L"2DShadowShader"));
	//pMtrl->SetPath(L"Material\\2DShadowMtrl.mtrl");
	//AddRes(L"Material\\2DShadowMtrl.mtrl", pMtrl);
}

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL *channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void *commanddata1, void *commanddata2);

void CResMgr::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);	
}
