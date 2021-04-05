#include "stdafx.h"
#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "ResMgr.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator2D.h"
#include "Animation2D.h"
#include "Collider2D.h"
#include "Light2D.h"
#include "Light3D.h"
#include "ParticleSystem.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Camera.h"

#include "CollisionMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"
#include "Device.h"
#include "Core.h"

#include "PlayerScript.h"
#include "MonsterScript.h"
#include "ToolCamScript.h"
#include "GridScript.h"

#include "meshdata.h"

#include "ResMgr.h"
#include "PathMgr.h"


CScene* CSceneMgr::GetCurScene()
{
	return m_pCurScene;
}

void CSceneMgr::ChangeScene(CScene * _pNextScene)
{
	SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;
}

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr)	
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}


void CSceneMgr::CreateTargetUI()
{
	Vec3 vScale(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.f);

	//Ptr<CTexture> arrTex[5] = { CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex")
	//	, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex")
	//	, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex")
	//	, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseLightTargetTex")
	//	, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularLightTargetTex") };

	//for (UINT i = 0; i < 5; ++i)
	//{
	//	CGameObject* pObject = new CGameObject;
	//	pObject->SetName(L"UI Object");
	//	pObject->FrustumCheck(false);	// 절두체 컬링 사용하지 않음
	//	pObject->AddComponent(new CTransform);
	//	pObject->AddComponent(new CMeshRender);

	//	// Transform 설정
	//	tResolution res = CRenderMgr::GetInst()->GetResolution();

	//	pObject->Transform()->SetLocalPos(Vec3(-(res.fWidth / 2.f) + (vScale.x / 2.f) + (i * vScale.x)
	//		, (res.fHeight / 2.f) - (vScale.y / 2.f)
	//		, 1.f));

	//	pObject->Transform()->SetLocalScale(vScale);

	//	// MeshRender 설정
	//	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	//	pObject->MeshRender()->SetMaterial(pMtrl->Clone());
	//	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, arrTex[i].GetPointer());

	//	// AddGameObject
	//	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
	//}
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"test5", L"Texture\\test5.png");

	CGameObject* pObject = new CGameObject;
	pObject->SetName(L"test");
	pObject->FrustumCheck(false);	// 절두체 컬링 사용하지 않음
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	// Transform 설정
	tResolution res = CRenderMgr::GetInst()->GetResolution();

	//pObject->Transform()->SetLocalPos(Vec3(-(res.fWidth / 2.f) + (vScale.x / 2.f)
	//									, (res.fHeight / 2.f) - (vScale.y / 2.f)
	//									, 1.f));
	pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1.f));

	pObject->Transform()->SetLocalScale(vScale);
			
	// MeshRender 설정
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	pObject->MeshRender()->SetMaterial(pMtrl->Clone());
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pTex.GetPointer());
		
	// AddGameObject
	m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
}

void CSceneMgr::init()
{
	// =================
	// 필요한 리소스 로딩
	// =================
	// Texture 로드
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");	
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>(L"Sky01", L"Texture\\Skybox\\Sky01.png");
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"Sky02", L"Texture\\Skybox\\Sky02.jpg");

	Ptr<CTexture> pColor = CResMgr::GetInst()->Load<CTexture>(L"Tile", L"Texture\\Tile\\TILE_03.tga");
	Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>(L"Tile_n", L"Texture\\Tile\\TILE_03_N.tga");

	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");

	// UAV 용 Texture 생성
	Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024
		, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, pSky01.GetPointer());

	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, pSky01.GetPointer());
	
	// ===============
	// Test Scene 생성
	// ===============
	m_pCurScene = new CScene;
	m_pCurScene->SetName(L"Test Scene");

	// ===============
	// Layer 이름 지정
	// ===============
	m_pCurScene->GetLayer(0)->SetName(L"Default");
	m_pCurScene->GetLayer(1)->SetName(L"Player");
	m_pCurScene->GetLayer(2)->SetName(L"Monster");
	m_pCurScene->GetLayer(3)->SetName(L"Bullet");

	m_pCurScene->GetLayer(30)->SetName(L"UI");
	m_pCurScene->GetLayer(31)->SetName(L"Tool");

	CGameObject* pObject = nullptr;
	CGameObject* pPlayerObject = nullptr;
	Ptr<CMeshData> pMeshData;

	// ====================
	// 3D Light Object 추가
	// ====================
	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);

	pObject->Light3D()->SetLightPos(Vec3(0.f, 500.f, 0.f));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(1000.f);

	pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	// ===================
	// Player 오브젝트 생성
	// ===================
	pPlayerObject = new CGameObject;

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Zombie1Death.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
	pPlayerObject = pMeshData->Instantiate();

	pPlayerObject->SetName(L"Player Object");
	pPlayerObject->AddComponent(new CTransform);
	//pPlayerObject->AddComponent(new CMeshRender);

	// Transform 설정
	pPlayerObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pPlayerObject->Transform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));

	//pPlayerObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, XM_PI));

	// MeshRender 설정
	//pPlayerObject->MeshRender()->SetDynamicShadow(true);
	//pPlayerObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pPlayerObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));
	//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
	//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());

	// Script 설정
	pPlayerObject->AddComponent(new CPlayerScript(pPlayerObject));

	// AddGameObject
	m_pCurScene->FindLayer(L"Player")->AddGameObject(pPlayerObject);


	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");

	//// ====================
	//// Animation 3D Object
	//// ====================
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"Monster");
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//// pObject->MeshRender()->SetDynamicShadow(true);		
	//m_pCurScene->AddGameObject(L"Default", pObject, false);

	// ==================
	// Camera Object 생성
	// ==================
	// Main Camera
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCam");
	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CToolCamScript(pPlayerObject));

	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetFar(100000.f);
	pMainCam->Camera()->SetLayerAllCheck();
	pMainCam->Camera()->SetLayerCheck(30, false);

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);

	// ====================
	// UI 오브젝트 생성
	// ====================
	// UI Camera
	CGameObject* pUICam = new CGameObject;
	pUICam->SetName(L"MainCam");
	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetFar(100.f);
	pUICam->Camera()->SetLayerCheck(30, true);
	pUICam->Camera()->SetWidth(CRenderMgr::GetInst()->GetResolution().fWidth);
	pUICam->Camera()->SetHeight(CRenderMgr::GetInst()->GetResolution().fHeight);

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pUICam);

	CreateTargetUI();

	// Main Camera
	pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCam");
	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CToolCamScript(pPlayerObject));

	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetFar(100000.f);
	pMainCam->Camera()->SetLayerAllCheck();
	pMainCam->Camera()->SetLayerCheck(30, false);

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);


	// ====================
	// Monster 오브젝트 생성
	// ====================

	pObject = new CGameObject;

	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Run.mdat", L"MeshData\\Zombie1Run.mdat");
	pObject = pMeshData->Instantiate();

	pObject->SetName(L"Monster Object");
	pObject->FrustumCheck(true);
	pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	// Transform 설정
	pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	//pObject->MeshRender()->SetDynamicShadow(true);
	/*pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 1);
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 2);
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 3);*/

	pObject->AddComponent(new CCollider2D);
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 100.f, 0.f));
	//pObject->Collider2D()->SetOffsetPos(Vec3(0.f, -5000.f, 0.f));
	pObject->Collider2D()->SetOffsetScale(Vec3(50.f, 0.f, 50.f));
	
	// Script 설정
	pObject->AddComponent(new CMonsterScript(pPlayerObject, pObject, m_pCurScene));

	// AddGameObject
	m_pCurScene->FindLayer(L"Monster")->AddGameObject(pObject);


	// ====================
	// Particle Object 생성
	// ====================
	//pObject = new CGameObject;
	//pObject->SetName(L"Particle");
	//pObject->AddComponent(new CTransform);	
	//pObject->AddComponent(new CParticleSystem);
	//
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(500.f, 0.f, 0.f));		
	//
	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	// ====================
	// Skybox 오브젝트 생성
	// ====================
	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	// MeshRender 설정
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky01.GetPointer());

	// AddGameObject
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	//// ========================
	//// Tessellation Test Object
	//// ========================
	//pObject = new CGameObject;
	//pObject->SetName(L"TessObject");

	//pObject->AddComponent(new CTransform);
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 100.f, 0.f));
	//pObject->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	//pObject->AddComponent(new CMeshRender);
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TessMtrl"));
	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	// ===================
	// Ground 오브젝트 생성
	// ===================
	pObject = new CGameObject;
	pObject->SetName(L"Ground Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	// Transform 설정
	pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(16000.f, 16000.f, 1.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	// MeshRender 설정
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());
	pObject->MeshRender()->SetDynamicShadow(true);

	// AddGameObject
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	//// ====================
	//// Grid 오브젝트 생성
	//// ====================
	//pObject = new CGameObject;
	//pObject->SetName(L"Grid");
	//pObject->FrustumCheck(false);
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CGridScript);

	//// Transform 설정
	//pObject->Transform()->SetLocalScale(Vec3(100000.f, 100000.f, 1.f));
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	//// MeshRender 설정
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"GridMtrl"));

	//// Script 설정	
	//pObject->GetScript<CGridScript>()->SetToolCamera(pMainCam);
	//pObject->GetScript<CGridScript>()->SetGridColor(Vec3(1.f, 1.f, 1.f));

	//// AddGameObject
	//m_pCurScene->FindLayer(L"Tool")->AddGameObject(pObject);

	// ==========================
	// Distortion Object 만들기
	// ==========================
	//pObject = new CGameObject;
	//pObject->SetName(L"PostEffect");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//// Material 값 셋팅
	//Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl");

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(pMtrl, 0);

	//pObject->Transform()->SetLocalScale(Vec3(500.f, 500.f, 500.f));
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 500.f, 0.f));

	//m_pCurScene->AddGameObject(L"Default", pObject, false);


	// ====================
	// Compute Shader Test
	// ====================
	int i = 1;

	Ptr<CMaterial> pCSMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"CSTestMtrl");
	pCSMtrl->SetData(SHADER_PARAM::INT_0, &i);
	CDevice::GetInst()->SetUAVToRegister_CS(pTestUAVTexture.GetPointer(), UAV_REGISTER::u0);

	pCSMtrl->Dispatch(1, 1024, 1); // --> 컴퓨트 쉐이더 수행	


	// =================================
	// Map 생성
	// =================================
	setMap();


	// =================================
	// CollisionMgr 충돌 그룹(Layer) 지정
	// =================================
	// Player Layer 와 Monster Layer 는 충돌 검사 진행
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Bullet", L"Monster");
	
	m_pCurScene->awake();
	m_pCurScene->start();
}

void CSceneMgr::update()
{		
	m_pCurScene->update();
	m_pCurScene->lateupdate();

	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();

	m_pCurScene->finalupdate();
	   
	// 충돌 처리
	CCollisionMgr::GetInst()->update();
}

void CSceneMgr::update_tool()
{
	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();
	m_pCurScene->finalupdate();
}

void CSceneMgr::FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
		for (size_t j = 0; j < vecObject.size(); ++j)
		{
			if (_strTag == vecObject[j]->GetName())
			{
				_vecFindObj.push_back(vecObject[j]);
			}
		}
	}	
}

bool Compare(CGameObject* _pLeft, CGameObject* _pRight)
{
	return (_pLeft->Transform()->GetWorldPos().z < _pRight->Transform()->GetWorldPos().z);
}

//void CSceneMgr::FindGameObjectByPoint(POINT _point, vector<CGameObject*>& _vecFindObj, CCamera* _pToolCam)
//{
//	CCamera* pCam = _pToolCam;
//	if (CCore::GetInst()->GetSceneMod() == SCENE_MOD::SCENE_PLAY)
//	{
//		pCam = CRenderMgr::GetInst()->GetCamera(0);
//	}
//
//	tResolution tRes = CRenderMgr::GetInst()->GetResolution();
//	Vec3 vPickPos = Vec3((float)_point.x - (tRes.fWidth / 2.f), (tRes.fHeight / 2.f) - (float)_point.y, 0.f);
//	vPickPos *= pCam->GetScale(); 
//	vPickPos += pCam->Transform()->GetWorldPos();
//
//	for (int i = 0; i < MAX_LAYER; ++i)
//	{
//		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
//		for (size_t j = 0; j < vecObject.size(); ++j)
//		{
//			if (vecObject[j]->Transform()->IsCasting(vPickPos))
//			{
//				_vecFindObj.push_back(vecObject[j]);
//			}
//		}
//	}
//
//	sort(_vecFindObj.begin(), _vecFindObj.end(), Compare);
//}

void CSceneMgr::setMap()
{
	// =================================
	// Map 생성
	// =================================
	CGameObject* pObject = nullptr;
	Ptr<CMeshData> pMeshData;
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Path2.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	
	//울타리 생성
	for (int i = 1; i < 260; i++)
	{
		pObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Fence.mdat", L"MeshData\\Fence.mdat");
		pObject = pMeshData->Instantiate();
		pObject->SetName(L"Fence");
		pObject->FrustumCheck(true);
		pObject->AddComponent(new CTransform);
		if (i < 65)
			pObject->Transform()->SetLocalPos(Vec3(5050.f, 0.f, -5050.f + 155 * i));
		else if (i < 130)
			pObject->Transform()->SetLocalPos(Vec3(-5050.f, 0.f, -5050.f + 155 * (i - 65)));
		else if (i < 195)
			pObject->Transform()->SetLocalPos(Vec3(-4930.f + 155 * (i - 130), 0.f, 5050.f));
		else
			pObject->Transform()->SetLocalPos(Vec3(-4940.f + 155 * (i - 195), 0.f, -5050.f));

		pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		if (i < 130)
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
		else
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0.f, 0.f));
		//pObject->MeshRender()->SetDynamicShadow(true);

		m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
	}

	//무덤 생성
	for (int i = 0; i < 9; i++)
	{
		pObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Tomb.mdat", L"MeshData\\Tomb.mdat");
		pObject = pMeshData->Instantiate();
		pObject->SetName(L"Tomb");
		pObject->FrustumCheck(true);
		pObject->AddComponent(new CTransform);

		if (i < 3)
		{
			pObject->Transform()->SetLocalPos(Vec3(-5500.f, 0.f, -3000 + (i * 3000)));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
		}
		else if (i < 6)
		{
			pObject->Transform()->SetLocalPos(Vec3(5500.f, 0.f, -3000 + ((i-3) * 3000)));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, XM_PI / 2, 0.f));
		}
		else
		{
			pObject->Transform()->SetLocalPos(Vec3(-3000 + ((i - 6) * 3000), 0.f,-5500.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, XM_PI, 0.f));
		}

		pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	
		//pObject->MeshRender()->SetDynamicShadow(true);

		m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
	}


	////그라운드 생성
	//for (int i = 0; i < 16; i++)
	//	for (int j = 0; j < 16; j++)
	//	{
	//		pObject = new CGameObject;
	//		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Ground.mdat", L"MeshData\\Ground.mdat");
	//		pObject = pMeshData->Instantiate();
	//		pObject->SetName(L"Ground");
	//		pObject->FrustumCheck(false);
	//		pObject->AddComponent(new CTransform);
	//		pObject->Transform()->SetLocalPos(Vec3(-8000 + i * 1000, 0.f, -8000 + j * 1000));
	//		pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
	//		pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//		//pObject->MeshRender()->SetDynamicShadow(true);
	//		m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
	//	}


	//길 생성
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (i < 3)
			{
				if ((i + j) % 2 == 0)
				{
					pObject = new CGameObject;
					pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path1.mdat", L"MeshData\\Path1.mdat");
					pObject = pMeshData->Instantiate();
					pObject->SetName(L"Path");
					pObject->FrustumCheck(true);
					pObject->AddComponent(new CTransform);
					pObject->Transform()->SetLocalPos(Vec3(-4800.f + j * 200, -7.f, -200 + i * 200));
					pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
					pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					//pObject->MeshRender()->SetDynamicShadow(true);
					m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
				}
				else
				{
					pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path2.mdat", L"MeshData\\Path2.mdat");
					pObject = pMeshData->Instantiate();
					pObject->SetName(L"Path");
					pObject->FrustumCheck(true);
					pObject->AddComponent(new CTransform);
					pObject->Transform()->SetLocalPos(Vec3(-4800.f + j * 200, -7.f, -200 + i * 200));
					pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
					pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					//pObject->MeshRender()->SetDynamicShadow(true);
					m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
				}
			}
			else
			{
				if (j < 23 || j > 25)
				{
					if (((i - 3 + j) % 2 == 0))
					{
						pObject = new CGameObject;
						pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path1.mdat", L"MeshData\\Path1.mdat");
						pObject = pMeshData->Instantiate();
						pObject->SetName(L"Path");
						pObject->FrustumCheck(true);
						pObject->AddComponent(new CTransform);
						pObject->Transform()->SetLocalPos(Vec3(-200 + (i - 3) * 200, -7.f, -4800.f + j * 200));
						pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
						pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
						//pObject->MeshRender()->SetDynamicShadow(true);
						m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
					}
					else
					{
						pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path2.mdat", L"MeshData\\Path2.mdat");
						pObject = pMeshData->Instantiate();
						pObject->SetName(L"Path");
						pObject->FrustumCheck(true);
						pObject->AddComponent(new CTransform);
						pObject->Transform()->SetLocalPos(Vec3(-200 + (i - 3) * 200, -7.f, -4800.f + j * 200));
						pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
						pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
						//pObject->MeshRender()->SetDynamicShadow(true);
						m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
					}
				}
			}
		}
	}
}