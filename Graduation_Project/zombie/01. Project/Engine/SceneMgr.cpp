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
#include "TParticleSystem.h"
#include "IParticleSystem.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Camera.h"

#include "CollisionMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"
#include "Device.h"
#include "Core.h"
#include "time.h"

#include "PlayerScript.h"
#include "MonsterScript.h"
#include "ToolCamScript.h"
#include "GridScript.h"
#include "StartSceneScript.h"
#include "meshdata.h"
#include "TombScript.h"
#include "ItemScript.h"
#include "BossScript.h"

#include "ResMgr.h"
#include "PathMgr.h"



CScene* CSceneMgr::GetCurScene()
{
	return m_pCurScene;
}

void CSceneMgr::ChangeScene(CScene* _pNextScene)
{
	SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;
}

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr)
{
	BulletCntArrTex[0] = (CResMgr::GetInst()->Load<CTexture>(L"infinite", L"Texture\\UI\\infinite.png"));
	BulletCntArrTex[1] = (CResMgr::GetInst()->Load<CTexture>(L"1", L"Texture\\UI\\1.png"));
	BulletCntArrTex[2] = (CResMgr::GetInst()->Load<CTexture>(L"2", L"Texture\\UI\\2.png"));
	BulletCntArrTex[3] = (CResMgr::GetInst()->Load<CTexture>(L"3", L"Texture\\UI\\3.png"));
	BulletCntArrTex[4] = (CResMgr::GetInst()->Load<CTexture>(L"4", L"Texture\\UI\\4.png"));
	BulletCntArrTex[5] = (CResMgr::GetInst()->Load<CTexture>(L"5", L"Texture\\UI\\5.png"));
	BulletCntArrTex[6] = (CResMgr::GetInst()->Load<CTexture>(L"6", L"Texture\\UI\\6.png"));
	BulletCntArrTex[7] = (CResMgr::GetInst()->Load<CTexture>(L"7", L"Texture\\UI\\7.png"));
	BulletCntArrTex[8] = (CResMgr::GetInst()->Load<CTexture>(L"8", L"Texture\\UI\\8.png"));
	BulletCntArrTex[9] = (CResMgr::GetInst()->Load<CTexture>(L"9", L"Texture\\UI\\9.png"));
	BulletCntArrTex[10] = (CResMgr::GetInst()->Load<CTexture>(L"10", L"Texture\\UI\\10.png"));
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}


void CSceneMgr::CreateTargetUI()
{
	tResolution res = CRenderMgr::GetInst()->GetResolution();
	Vec3 vScale(res.fWidth, res.fHeight, 1.f);
	// Transform 설정
	hpBarWidth = CRenderMgr::GetInst()->GetResolution().fWidth / 4.1;
	hpBarHeight = CRenderMgr::GetInst()->GetResolution().fHeight / 45;

	GameSceneArrTex[0] = (CResMgr::GetInst()->Load<CTexture>(L"BGUI", L"Texture\\UI\\BGUI.png"));
	GameSceneArrTex[1] = (CResMgr::GetInst()->Load<CTexture>(L"MiniMap", L"Texture\\UI\\MiniMap.png"));
	GameSceneArrTex[2] = (CResMgr::GetInst()->Load<CTexture>(L"miniMapPlayer", L"Texture\\UI\\playerTest.png"));
	GameSceneArrTex[3] = (CResMgr::GetInst()->Load<CTexture>(L"Quest", L"Texture\\UI\\Quest.png"));
	GameSceneArrTex[4] = (CResMgr::GetInst()->Load<CTexture>(L"Picture", L"Texture\\UI\\PlayerPicture.png"));
	GameSceneArrTex[5] = (CResMgr::GetInst()->Load<CTexture>(L"HpBarEdge", L"Texture\\UI\\HpBarEdge.png"));
	GameSceneArrTex[6] = (CResMgr::GetInst()->Load<CTexture>(L"HpBar", L"Texture\\UI\\HpBar.png"));
	GameSceneArrTex[7] = (CResMgr::GetInst()->Load<CTexture>(L"PowerBuffOn", L"Texture\\UI\\PowerBuffOn.png"));
	GameSceneArrTex[8] = (CResMgr::GetInst()->Load<CTexture>(L"PowerBuffOff", L"Texture\\UI\\PowerBuffOff.png"));
	GameSceneArrTex[9] = (CResMgr::GetInst()->Load<CTexture>(L"DefenceBuffOn", L"Texture\\UI\\DefenceBuffOn.png"));
	GameSceneArrTex[10] = (CResMgr::GetInst()->Load<CTexture>(L"DefenceBuffOff", L"Texture\\UI\\DefenceBuffOff.png"));
	GameSceneArrTex[11] = (CResMgr::GetInst()->Load<CTexture>(L"SpeedBuffOn", L"Texture\\UI\\SpeedBuffOn.png"));
	GameSceneArrTex[12] = (CResMgr::GetInst()->Load<CTexture>(L"SpeedBuffOff", L"Texture\\UI\\SpeedBuffOff.png"));
	GameSceneArrTex[13] = (CResMgr::GetInst()->Load<CTexture>(L"infinite", L"Texture\\UI\\infinite.png"));

	Ptr<CTexture> StartSceneArrTex[1] = { CResMgr::GetInst()->Load<CTexture>(L"StartBG", L"Texture\\UI\\StartBG.png") };

	Ptr<CTexture> GameClearSceneArrTex[1] = { CResMgr::GetInst()->Load<CTexture>(L"GameClearBG", L"Texture\\UI\\GameClearBG.png") };

	Ptr<CTexture> GameOverSceneArrTex[1] = { CResMgr::GetInst()->Load<CTexture>(L"GameOverBG", L"Texture\\UI\\GameOverBG.png") };

	int NumgameSceneUI = 14;
	int NumStartSceneUI = 1;
	int NumGameClearSceneUI = 1;
	int NumGameOVerSceneUI = 1;

	if (SceneState == SCENE_STATE::GAME_SCENE)
	{
		for (UINT i = 0; i < NumgameSceneUI; i++)
		{
			CGameObject* pObject = new CGameObject;
			pObject->FrustumCheck(true);	// 절두체 컬링 사용하지 않음
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CMeshRender);

			if (i == 0)
			{
				pObject->SetName(L"BGUI");
				pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 5.0f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x, vScale.y, 1.f));
			}

			else if (i == 1)
			{
				pObject->SetName(L"MiniMapUI");
				/*pObject->Transform()->SetLocalPos(Vec3((res.fWidth / 2.f) - ((vScale.x / miniMapUIRatio) / 2)
					, (res.fHeight / 2.f) - ((vScale.y / miniMapUIRatio) * (vScale.x / vScale.y)  / 2.f)
					, 1.5f));*/
				pObject->Transform()->SetLocalPos(Vec3(-(res.fWidth / 2.f) + ((vScale.x / miniMapUIRatio) / 2) + vScale.x * 0.015
					, -(res.fHeight / 2.f) + (((vScale.y / miniMapUIRatio) * (vScale.x / vScale.y)) / 2) + vScale.y * 0.015
					, 2.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / miniMapUIRatio, (vScale.y / miniMapUIRatio) * (vScale.x / vScale.y), 1.f));
			}

			else if (i == 2)
			{
				pObject->SetName(L"PlayerPosUI");

				Vec3 playerPos = m_pPlayerArr[playerID]->Transform()->GetLocalPos();
				playerPos = Vec3(playerPos.x + 5000.f, 0.f, playerPos.z + 5000.f);
				Vec2 posRatio = Vec2(playerPos.x / 10000.f * ((vScale.x / miniMapUIRatio)), playerPos.z / 10000.f * ((vScale.y / miniMapUIRatio) * (vScale.x / vScale.y)));
				pObject->Transform()->SetLocalPos(Vec3(-(res.fWidth / 2.f) + vScale.x * 0.015 + posRatio.x
					, -(res.fHeight / 2.f) + vScale.y * 0.015 + posRatio.y
					, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / posUIRatio, (vScale.y / posUIRatio) * (vScale.x / vScale.y), 1.f));
			}

			else if (i == 3)
			{
				pObject->SetName(L"QuestUI");
				pObject->Transform()->SetLocalPos(Vec3(0, res.fHeight / 8, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 2, (vScale.y / 2), 1.f));
			}

			else if (i == 4)
			{
				pObject->SetName(L"HpBarEdgeUI");
				pObject->Transform()->SetLocalPos(Vec3(res.fWidth / 5.10, -res.fHeight / 2.75, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 6.8, (vScale.y / 7), 1.f));
			}

			else if (i == 5)
			{
				pObject->SetName(L"HpBarEdgeeUI");
				pObject->Transform()->SetLocalPos(Vec3(-res.fWidth / 20, -res.fHeight / 2.75, 1.2f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 4, (vScale.y / 40), 1.f));
			}

			else if (i == 6)
			{
				pObject->SetName(L"HpBarUI");
				pObject->Transform()->SetLocalPos(Vec3(-res.fWidth / 20, -res.fHeight / 2.75, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(hpBarWidth, hpBarHeight, 1.f));
			}

			else if (i == 7)
			{
				pObject->SetName(L"PowerBuffOnUI");
				pObject->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 8)
			{
				pObject->SetName(L"PowerBuffOffUI");
				pObject->Transform()->SetLocalPos(Vec3(-res.fWidth / 6.5, -res.fHeight / 2.3, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 9)
			{
				pObject->SetName(L"DefenceBuffOnUI");
				pObject->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 10)
			{
				pObject->SetName(L"DefenceBuffOffUI");
				pObject->Transform()->SetLocalPos(Vec3(-res.fWidth / 10, -res.fHeight / 2.3, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 11)
			{
				pObject->SetName(L"SpeedBuffOnUI");
				pObject->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 12)
			{
				pObject->SetName(L"SpeedBuffOffUI");
				pObject->Transform()->SetLocalPos(Vec3(-res.fWidth / (130 / 6), -res.fHeight / 2.3, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			else if (i == 13)
			{
				pObject->SetName(L"BulletCnt_UI");
				pObject->Transform()->SetLocalPos(Vec3(res.fWidth / 2.2, -res.fHeight / 2.7, 1.f));
				pObject->Transform()->SetLocalScale(Vec3(vScale.x / 25, (vScale.y / 25) * res.fWidth / res.fHeight, 1.f));
			}

			// MeshRender 설정
			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
			pObject->MeshRender()->SetMaterial(pMtrl->Clone());
			pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, GameSceneArrTex[i].GetPointer());

			// AddGameObject
			m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else if (SceneState == SCENE_STATE::START_SCENE)
	{
		for (UINT i = 0; i < NumStartSceneUI; i++)
		{
			CGameObject* pObject = new CGameObject;
			pObject->SetName(L"StartBgUI");
			pObject->FrustumCheck(true);	// 절두체 컬링 사용하지 않음
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CMeshRender);

			// Transform 설정
			tResolution res = CRenderMgr::GetInst()->GetResolution();

			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1.f));

			pObject->Transform()->SetLocalScale(vScale);

			// MeshRender 설정
			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
			pObject->MeshRender()->SetMaterial(pMtrl->Clone());
			pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, StartSceneArrTex[i].GetPointer());

			// AddGameObject
			m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else if (SceneState == SCENE_STATE::GAMECLEAR_SCENE)
	{
		for (UINT i = 0; i < NumGameClearSceneUI; i++)
		{
			CGameObject* pObject = new CGameObject;
			pObject->SetName(L"EndBgUI");
			pObject->FrustumCheck(true);	// 절두체 컬링 사용하지 않음
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CMeshRender);

			// Transform 설정
			tResolution res = CRenderMgr::GetInst()->GetResolution();

			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1.f));

			pObject->Transform()->SetLocalScale(vScale);

			// MeshRender 설정
			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
			pObject->MeshRender()->SetMaterial(pMtrl->Clone());
			pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, GameClearSceneArrTex[i].GetPointer());

			// AddGameObject
			m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
		}
	}

	else if (SceneState == SCENE_STATE::GAMEOVER_SCENE)
	{
		for (UINT i = 0; i < NumGameOVerSceneUI; i++)
		{
			CGameObject* pObject = new CGameObject;
			pObject->SetName(L"EndBgUI");
			pObject->FrustumCheck(true);	// 절두체 컬링 사용하지 않음
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CMeshRender);

			// Transform 설정
			tResolution res = CRenderMgr::GetInst()->GetResolution();

			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1.f));

			pObject->Transform()->SetLocalScale(vScale);

			// MeshRender 설정
			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
			pObject->MeshRender()->SetMaterial(pMtrl->Clone());
			pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, GameOverSceneArrTex[i].GetPointer());

			// AddGameObject
			m_pCurScene->FindLayer(L"UI")->AddGameObject(pObject);
		}
	}
}



void CSceneMgr::init()
{
	//initStartScene();
	initGameScene();
	//initEndScene();

	//m_pCurScene = m_pStartScene;
	//m_pCurScene = m_pCurScene;

	m_pCurScene->awake();
	m_pCurScene->start();
}

void CSceneMgr::initGameScene()
{
	// ===============
	// GameScene 생성
	// ===============
	m_pCurScene = new CScene;

	// ===============
	// Layer 이름 지정
	// ===============
	m_pCurScene->GetLayer(0)->SetName(L"Default");
	m_pCurScene->GetLayer(1)->SetName(L"Player");
	m_pCurScene->GetLayer(2)->SetName(L"Monster");
	m_pCurScene->GetLayer(3)->SetName(L"Bullet");
	m_pCurScene->GetLayer(4)->SetName(L"Tomb");
	m_pCurScene->GetLayer(5)->SetName(L"Item");
	m_pCurScene->GetLayer(6)->SetName(L"Boss");

	m_pCurScene->GetLayer(30)->SetName(L"UI");
	m_pCurScene->GetLayer(31)->SetName(L"Tool");

	CGameObject* pObject = nullptr;
	CGameObject* pPlayerObject = nullptr;
	Ptr<CMeshData> pMeshData;


	if (SceneState == SCENE_STATE::GAME_SCENE)
	{
		// =================
		// 필요한 리소스 로딩
		// =================
		// Texture 로드
		Ptr<CTexture> pColor = CResMgr::GetInst()->Load<CTexture>(L"Ground", L"Texture\\Ground\\Ground.tga");
		Ptr<CTexture> pNormal = CResMgr::GetInst()->Load<CTexture>(L"Ground_n", L"Texture\\Ground\\Ground_n.tga");

		// UAV 용 Texture 생성
		Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		// ===============
		// GameScene 생성
		// ===============
		m_pCurScene->SetName(L"Game Scene");

		// ====================
		// 3D Light Object 추가
		// ====================
		m_Light = new CGameObject;
		m_Light->AddComponent(new CTransform);
		m_Light->AddComponent(new CLight3D);

		m_Light->Light3D()->SetLightPos(Vec3(0.f, 500.f, 0.f));
		m_Light->Light3D()->SetLightType(LIGHT_TYPE::DIR);
		m_Light->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
		m_Light->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
		m_Light->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		m_Light->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
		m_Light->Light3D()->SetLightRange(10000.f);

		m_Light->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
		m_pCurScene->FindLayer(L"Default")->AddGameObject(m_Light);


		// ===================
		// Player 오브젝트 생성
		// ===================
	/*	pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\HealPack.fbx");
		pMeshData->Save(pMeshData->GetPath());*/

		for (int i = 0; i < playerNum; i++)
		{
			m_pPlayerArr[i] = new CGameObject;

			// 모델을 플레이어별로 따로 설정할수도 있음
			// 아직 보류
			pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
			m_pPlayerArr[i] = pMeshData->Instantiate();

			m_pPlayerArr[i]->SetName(L"Player Object");
			m_pPlayerArr[i]->AddComponent(new CTransform);
			//pPlayerObject->AddComponent(new CMeshRender);

			// Transform 설정

			if (i == 0)
				m_pPlayerArr[i]->Transform()->SetLocalPos(Vec3(-200.f, 0.f, 200.f));
			else if (i == 1)
				m_pPlayerArr[i]->Transform()->SetLocalPos(Vec3(200.f, 0.f, 200.f));
			else if (i == 2)
				m_pPlayerArr[i]->Transform()->SetLocalPos(Vec3(-200.f, 0.f, -200.f));
			else if (i == 3)
				m_pPlayerArr[i]->Transform()->SetLocalPos(Vec3(200.f, 0.f, -200.f));

			m_pPlayerArr[i]->Transform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));

			//pPlayerObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, XM_PI));

			// MeshRender 설정
			m_pPlayerArr[i]->MeshRender()->SetDynamicShadow(true);
			//pPlayerObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
			//pPlayerObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TestMtrl"));
			//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
			//pPlayerObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());

			m_pPlayerArr[i]->AddComponent(new CCollider2D);
			m_pPlayerArr[i]->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
			m_pPlayerArr[i]->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f + collOffset, 0.f));
			m_pPlayerArr[i]->Collider2D()->SetOffsetScale(Vec3(100.f, 0.f, 100.f));

			// Script 설정
			// 플레이어 일시
			if (i == playerID)
			{
				m_pPlayerArr[i]->AddComponent(new CPlayerScript(m_pPlayerArr[i], true));
			}
			else
			{
				m_pPlayerArr[i]->AddComponent(new CPlayerScript(m_pPlayerArr[i], false));
				//m_pPlayerArr[i]->GetScript<CPlayerScript>()->GetStatus()->isDisappear = true;
				//m_pPlayerArr[i]->Transform()->SetLocalPos(Vec3(-20000.f, -20000.f, -20000.f));
			}

			m_pPlayerArr[i]->GetScript<CPlayerScript>()->SetBulletCollOffset(collOffset);


			// AddGameObject
			m_pCurScene->FindLayer(L"Player")->AddGameObject(m_pPlayerArr[i]);

		}

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

		// ===================
		// Boss 오브젝트 생성
		// ===================
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\BossAttack.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		pObject = new CGameObject;

		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\BossAttack.mdat", L"MeshData\\BossAttack.mdat");
		pObject = pMeshData->Instantiate();

		pObject->SetName(L"Boss Object");
		pObject->FrustumCheck(true);
		pObject->AddComponent(new CTransform);

		pObject->Transform()->SetLocalPos(Vec3(-40000.f, 0.f, 0));
		pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0, 0));

		pObject->AddComponent(new CCollider2D);
		pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
		pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 50.f + collOffset));

		//pObject->Collider2D()->SetOffsetPos(Vec3(0.f, -5000.f, 0.f));
		pObject->Collider2D()->SetOffsetScale(Vec3(100.f, 0.f, 100.f));

		pObject->MeshRender()->SetDynamicShadow(true);

		// Script 설정
		pObject->AddComponent(new CBossScript(m_pPlayerArr, playerNum, pObject, m_pCurScene));

		m_pCurScene->FindLayer(L"Boss")->AddGameObject(pObject);



		// ==================
		// Camera Object 생성
		// ==================
		// Main Camera
		CGameObject* pMainCam = new CGameObject;
		pMainCam->SetName(L"MainCam");
		pMainCam->AddComponent(new CTransform);
		pMainCam->AddComponent(new CCamera);
		pMainCam->AddComponent(new CToolCamScript(m_pPlayerArr[0]));

		pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		pMainCam->Camera()->SetFar(100000.f);
		pMainCam->Camera()->SetLayerAllCheck();
		pMainCam->Camera()->SetLayerCheck(30, false);

		m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);

		//====================
		//UI 오브젝트 생성
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
		pMainCam->AddComponent(new CToolCamScript(m_pPlayerArr[0]));

		pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		pMainCam->Camera()->SetFar(100000.f);
		pMainCam->Camera()->SetLayerAllCheck();
		pMainCam->Camera()->SetLayerCheck(30, false);

		m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);


		//////////
	/*	pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\SpPotion.fbx");
		pMeshData->Save(pMeshData->GetPath());*/

		//pObject = new CGameObject;

		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\HpPotion.mdat", L"MeshData\\HpPotion.mdat");
		//pObject = pMeshData->Instantiate();

		//pObject->SetName(L"ItemBox Object");
		//pObject->FrustumCheck(false);
		//pObject->AddComponent(new CTransform);
		//pObject->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));
		//pObject->Transform()->SetLocalScale(Vec3(1, 1, 1));
		//pObject->Transform()->SetLocalPos(Vec3(0.f, 50.f, 0.f));
		//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
		/////////////
		//pObject = new CGameObject;

		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SpPotion.mdat", L"MeshData\\SpPotion.mdat");
		//pObject = pMeshData->Instantiate();

		//pObject->SetName(L"ItemBox Object");
		//pObject->FrustumCheck(false);
		//pObject->AddComponent(new CTransform);
		//pObject->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));
		//pObject->Transform()->SetLocalScale(Vec3(1, 1, 1));
		//pObject->Transform()->SetLocalPos(Vec3(100.f, 50.f, 0.f));
		//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
		///////////

		//pObject = new CGameObject;

		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\DfPotion.mdat", L"MeshData\\DfPotion.mdat");
		//pObject = pMeshData->Instantiate();

		//pObject->SetName(L"ItemBox Object");
		//pObject->FrustumCheck(false);
		//pObject->AddComponent(new CTransform);
		//pObject->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));
		//pObject->Transform()->SetLocalScale(Vec3(1, 1, 1));
		//pObject->Transform()->SetLocalPos(Vec3(200.f, 50.f, 0.f));
		//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
		/////////

		// ====================
		// Monster 오브젝트 생성
		// ====================
		for (int i = 0; i < monsterCnt; i++)
		{
			pObject = new CGameObject;

			pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Zombie1Run.mdat", L"MeshData\\Zombie1Run.mdat");
			pObject = pMeshData->Instantiate();

			pObject->SetName(L"Monster Object");
			pObject->FrustumCheck(true);
			pObject->AddComponent(new CTransform);
			//pObject->AddComponent(new CMeshRender);

			// Transform 설정
			bool isRightPos = false;
			float randomXPos;
			float randomZPos;

			// 좀비 초기 위치 설정
			while (!isRightPos)
			{
				randomXPos = rand() % 9000 - 4500;
				randomZPos = rand() % 9000 - 4500;

				if (randomXPos >= -1500 && randomXPos <= 1500 && randomZPos >= -1500 && randomZPos <= 1500)
					isRightPos = false;

				else
					isRightPos = true;
			}

			pObject->Transform()->SetLocalPos(Vec3(randomXPos, 0.f, randomZPos));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			pObject->MeshRender()->SetDynamicShadow(true);
			/*pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 1);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 2);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 3);*/

			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f + collOffset, 0.f));
			//pObject->Collider2D()->SetOffsetPos(Vec3(0.f, -5000.f, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(50.f, 0.f, 50.f));

			// Script 설정
			pObject->AddComponent(new CMonsterScript(m_pPlayerArr, playerNum, pObject, m_pCurScene));

			// AddGameObject
			m_pCurScene->FindLayer(L"Monster")->AddGameObject(pObject);

			monsterArr[i] = pObject;
		}


		// ====================
		// Item 생성
		// ====================
		for (int i = 0; i < ItemCnt; i++)
		{
			pObject = new CGameObject;

			if (i < 10)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\PowerPotion.mdat", L"MeshData\\PowerPotion.mdat");
			else if (i < 20)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\DfPotion.mdat", L"MeshData\\DfPotion.mdat");
			else if (i < 30)
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SpPotion.mdat", L"MeshData\\SpPotion.mdat");
			else if (i < 40)
			{
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\HealPack.mdat", L"MeshData\\HealPack.mdat");
				//pObject->Transform()->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
			}
			else if (i < 50)
			{
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\RandomBox.mdat", L"MeshData\\RandomBox.mdat");
			}
			pObject = pMeshData->Instantiate();

			pObject->SetName(L"Item Object");
			pObject->FrustumCheck(true);
			pObject->AddComponent(new CTransform);

			pObject->Transform()->SetLocalPos(Vec3(20000.f, 0.f, 20000.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

			//pObject->MeshRender()->SetDynamicShadow(true);
			/*pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 1);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 2);
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 3);*/

			pObject->AddComponent(new CCollider2D);
			pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
			pObject->Collider2D()->SetOffsetPos(Vec3(0.f, collOffset, 0.f));
			pObject->Collider2D()->SetOffsetScale(Vec3(50.f, 0.f, 50.f));

			pObject->MeshRender()->SetDynamicShadow(true);

			// Script 설정
			if (i < 10)
				pObject->AddComponent(new CItemScript(ItemState::I_PwPotion));
			else if (i < 20)
				pObject->AddComponent(new CItemScript(ItemState::I_DfPotion));
			else if (i < 30)
				pObject->AddComponent(new CItemScript(ItemState::I_SpPotion));
			else if (i < 40)
			{
				pObject->AddComponent(new CItemScript(ItemState::I_HpItem));
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0.f, 0.f));
				pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, collOffset));
				pObject->Collider2D()->SetOffsetScale(Vec3(350.f, 0.f, 500.f));
			}
			else if (i < 50)
			{
				pObject->AddComponent(new CItemScript(ItemState::I_BulletItem));
				pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, XM_PI, 0));
				pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, collOffset));
				pObject->Collider2D()->SetOffsetScale(Vec3(150.f, 0.f, 150.f));

			}
			// AddGameObject
			m_pCurScene->FindLayer(L"Item")->AddGameObject(pObject);

			m_pItem[i] = pObject;
		}

		// ====================
		// Game Manager 생성
		// ====================
		m_pGameManager = new GameMgr(m_pCurScene, monsterArr, monsterCnt, m_pPlayerArr, playerNum, playerID);


		// ====================
		// Particle Object 생성
		// ====================
		pObject = new CGameObject;
		pObject->SetName(L"Particle");
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CParticleSystem);

		pObject->FrustumCheck(false);
		pObject->Transform()->SetLocalPos(Vec3(0.f, 50.f, 0.f));

		m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


		//// ====================
		//// Skybox 오브젝트 생성
		//// ====================
		//pObject = new CGameObject;
		//pObject->SetName(L"SkyBox");
		//pObject->FrustumCheck(false);
		//pObject->AddComponent(new CTransform);
		//pObject->AddComponent(new CMeshRender);

		//// MeshRender 설정
		//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		//pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
		//pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky01.GetPointer());

		//// AddGameObject
		//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


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
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				pObject = new CGameObject;
				pObject->SetName(L"Ground Object");
				pObject->AddComponent(new CTransform);
				pObject->AddComponent(new CMeshRender);

				// Transform 설정
				pObject->Transform()->SetLocalPos(Vec3(-6000 + i * 1000, -8.f, -6000 + j * 1000));
				pObject->Transform()->SetLocalScale(Vec3(1000.f, 1000.f, 1.f));
				pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

				// MeshRender 설정
				pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
				pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));
				pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
				pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());

				// AddGameObject
				m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
			}


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
		//Distortion Object 만들기
		//==========================
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
		CCollisionMgr::GetInst()->CheckCollisionLayer(L"Monster", L"Monster");
		CCollisionMgr::GetInst()->CheckCollisionLayer(L"Bullet", L"Tomb");
		CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Item");
		CCollisionMgr::GetInst()->CheckCollisionLayer(L"Monster", L"Player");
		CCollisionMgr::GetInst()->CheckCollisionLayer(L"Bullet", L"Boss");
		//m_pCurScene->awake();
		//m_pCurScene->start();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else if (SceneState == SCENE_STATE::START_SCENE)
	{
		// ===============
		// StartScene 생성
		// ===============
		m_pCurScene->SetName(L"Start Scene");

		// ===================
		// Player 오브젝트 생성
		// ===================

		pPlayerObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
		pPlayerObject = pMeshData->Instantiate();
		pPlayerObject->SetName(L"Player Object");
		pPlayerObject->AddComponent(new CTransform);
		pPlayerObject->Transform()->SetLocalPos(Vec3(-200.f, 0.f, 200.f));
		pPlayerObject->AddComponent(new CPlayerScript(pPlayerObject, false));
		m_pCurScene->FindLayer(L"Player")->AddGameObject(pPlayerObject);

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

		//====================
		//UI 오브젝트 생성
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
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else if (SceneState == SCENE_STATE::GAMECLEAR_SCENE)
	{
		// ===============
		// GameClearScene 생성
		// ===============
		m_pCurScene->SetName(L"GameClear Scene");

		// ===================
		// Player 오브젝트 생성
		// ===================
		pPlayerObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
		pPlayerObject = pMeshData->Instantiate();
		pPlayerObject->SetName(L"Player Object");
		pPlayerObject->AddComponent(new CTransform);
		pPlayerObject->Transform()->SetLocalPos(Vec3(-200.f, 0.f, 200.f));
		pPlayerObject->AddComponent(new CPlayerScript(pPlayerObject, false));

		// AddGameObject
		m_pCurScene->FindLayer(L"Player")->AddGameObject(pPlayerObject);

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

		//====================
		//UI 오브젝트 생성
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

	}

	else if (SceneState == SCENE_STATE::GAMEOVER_SCENE)
	{
		// ===============
		// GameOverScene 생성
		// ===============
		m_pCurScene->SetName(L"GameOver Scene");

		// ===================
		// Player 오브젝트 생성
		// ===================
		pPlayerObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SoldierIdle.mdat", L"MeshData\\SoldierIdle.mdat");
		pPlayerObject = pMeshData->Instantiate();
		pPlayerObject->SetName(L"Player Object");
		pPlayerObject->AddComponent(new CTransform);
		pPlayerObject->Transform()->SetLocalPos(Vec3(-200.f, 0.f, 200.f));
		pPlayerObject->AddComponent(new CPlayerScript(pPlayerObject, false));

		// AddGameObject
		m_pCurScene->FindLayer(L"Player")->AddGameObject(pPlayerObject);

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

		//====================
		//UI 오브젝트 생성
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

	}
}

void CSceneMgr::initStartScene()
{
	//// ===============
	//// GameScene 생성
	//// ===============
	//m_pStartScene = new CScene;
	//m_pStartScene->SetName(L"Start Scene");
	//m_pCurScene = m_pStartScene;

	//// ===============
	//// Layer 이름 지정
	//// ===============
	//m_pStartScene->GetLayer(0)->SetName(L"Default");
	//m_pStartScene->GetLayer(1)->SetName(L"Player");
	//m_pStartScene->GetLayer(2)->SetName(L"Monster");
	//m_pStartScene->GetLayer(3)->SetName(L"Bullet");
	//m_pStartScene->GetLayer(4)->SetName(L"Tomb");

	//m_pStartScene->GetLayer(30)->SetName(L"UI");
	//m_pStartScene->GetLayer(31)->SetName(L"Tool");

	//CGameObject* pObject = nullptr;

	////====================
	////UI 오브젝트 생성
	//// ====================
	//// UI Camera
	//CGameObject* pUICam = new CGameObject;
	//pUICam->SetName(L"MainCam");
	//pUICam->AddComponent(new CTransform);
	//pUICam->AddComponent(new CCamera);

	//pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	//pUICam->Camera()->SetFar(100.f);
	//pUICam->Camera()->SetLayerCheck(30, true);
	//pUICam->Camera()->SetWidth(CRenderMgr::GetInst()->GetResolution().fWidth);
	//pUICam->Camera()->SetHeight(CRenderMgr::GetInst()->GetResolution().fHeight);

	//m_pStartScene->FindLayer(L"Default")->AddGameObject(pUICam);

	//Vec3 vScale(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.f);

	//// UI Object
	//Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"BlockingView", L"Texture\\BlockingView.png");

	//pObject = new CGameObject;
	//pObject->SetName(L"Test1");
	//pObject->FrustumCheck(false);	// 절두체 컬링 사용하지 않음
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//// Transform 설정
	//tResolution res = CRenderMgr::GetInst()->GetResolution();

	////pObject->Transform()->SetLocalPos(Vec3(-(res.fWidth / 2.f) + (vScale.x / 2.f)
	////									, (res.fHeight / 2.f) - (vScale.y / 2.f)
	////									, 1.f));
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 1.0f));

	//pObject->Transform()->SetLocalScale(vScale);

	//// MeshRender 설정
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"TexMtrl");
	//pObject->MeshRender()->SetMaterial(pMtrl->Clone());
	//pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pTex.GetPointer());
	////pObject->AddComponent(new CStartSceneScript(this));
	//
	//// AddGameObject
	//m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject);



	////pObject = new CGameObject;
	////m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject);
	////m_pStartScene->FindLayer(L"Monster")->AddGameObject(pObject);
	////m_pStartScene->FindLayer(L"Bullet")->AddGameObject(pObject);
	////m_pStartScene->FindLayer(L"Tomb")->AddGameObject(pObject);
	////m_pStartScene->FindLayer(L"Tool")->AddGameObject(pObject);

	////m_pStartScene->awake();
	////m_pStartScene->start();
}

void CSceneMgr::initEndScene()
{
}

void CSceneMgr::update()
{
	// GameScene 업데이트
	m_pCurScene->update();
	m_pCurScene->lateupdate();

	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();

	m_pCurScene->finalupdate();

	// 충돌 처리
	CCollisionMgr::GetInst()->update();


	if (KEY_TAB(KEY_TYPE::KEY_0))
	{
		//SAFE_DELETE(m_pCurScene);
		m_pCurScene = new CScene;
		//delete m_pCurScene;
		SceneState = SCENE_STATE::START_SCENE;
		init();

		isChange = true;
	}



	if (SceneState == SCENE_STATE::START_SCENE)
	{
		if (KEY_TAB(KEY_TYPE::KEY_SPACE))
		{
			//SAFE_DELETE(m_pCurScene);
			m_pCurScene = new CScene;
			//delete m_pCurScene;
			SceneState = SCENE_STATE::GAME_SCENE;

			// 변수값 초기화
			initValue();

			init();

			isChange = true;
		}
	}

	else if (SceneState == SCENE_STATE::GAME_SCENE)
	{
		// 게임 매니저 업데이트
		m_pGameManager->GameMgrUpdate();

		// 조명 위치 업데이트
		Vec3 pPos = m_pPlayerArr[playerID]->Transform()->GetLocalPos();
		m_Light->Light3D()->updateCameraTransform(Vec3(pPos.x, 2600, pPos.z));

		// 무덤 파괴
		if (KEY_TAB(KEY_TYPE::KEY_8))
		{
			m_pGameManager->cheatDestroyTomb();
		}

		// 보스 소환
		if (KEY_TAB(KEY_TYPE::KEY_9))
		{
			m_pGameManager->cheatBossSpawn();
		}


		if (m_pGameManager->GetGameClear())
		{
			//SAFE_DELETE(m_pCurScene);
			m_pCurScene = new CScene;

			SceneState = SCENE_STATE::GAMECLEAR_SCENE;
			init();

			isChange = true;
		}

		else if (m_pGameManager->GetGameOver())
		{
			//SAFE_DELETE(m_pCurScene);
			m_pCurScene = new CScene;

			SceneState = SCENE_STATE::GAMEOVER_SCENE;
			init();

			isChange = true;
		}

		if (KEY_TAB(KEY_TYPE::KEY_I))
		{
			pressI = true;
			QuestVisible = !QuestVisible;
		}
	}

	else if (SceneState == SCENE_STATE::GAMECLEAR_SCENE)
	{
		if (KEY_TAB(KEY_TYPE::KEY_SPACE))
		{
			//SAFE_DELETE(m_pCurScene);
			m_pCurScene = new CScene;
			//delete m_pCurScene;
			SceneState = SCENE_STATE::START_SCENE;
			init();

			isChange = true;
		}
	}

	else if (SceneState == SCENE_STATE::GAMEOVER_SCENE)
	{
		if (KEY_TAB(KEY_TYPE::KEY_SPACE))
		{
			//SAFE_DELETE(m_pCurScene);
			m_pCurScene = new CScene;
			//delete m_pCurScene;
			SceneState = SCENE_STATE::START_SCENE;
			init();

			isChange = true;
		}
	}

	//if (SceneState == SCENE_STATE::GAME_SCENE)
	//{
	//	// 플레이어 오프셋 조정
	//	for (int i = 0; i < MAX_LAYER; ++i)
	//	{
	//		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
	//		for (size_t j = 0; j < vecObject.size(); ++j)
	//		{
	//			if (L"Player Object" == vecObject[j]->GetName())
	//			{
	//				float offset = vecObject[j]->GetScript<CPlayerScript>()->GetCollOffset();
	//				vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, offset + collOffset, 0.f));
	//			}
	//		}
	//	}
	//}

	// 충돌 처리 Offset 변경
	if (KEY_TAB(KEY_TYPE::KEY_C) && SceneState == SCENE_STATE::GAME_SCENE)
	{
		if (collOffset == 0)
		{
			collOffset = 20000.f;
		}
		else
		{
			collOffset = 0.f;
		}

		// 플레이어 Offset 변경
		m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->SetBulletCollOffset(collOffset);

		for (int i = 0; i < MAX_LAYER; ++i)
		{
			const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				if (L"Player Object" == vecObject[j]->GetName())
				{
					float offset = vecObject[j]->GetScript<CPlayerScript>()->GetCollOffset();
					vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, offset + collOffset, 0.f));
					//vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, 50000.f + collOffset, 0.f));
				}

				// 몬스터 Offset 변경
				else if (L"Monster Object" == vecObject[j]->GetName())
				{
					vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, 50.f + collOffset, 0.f));
				}

				else if (L"Tomb Object" == vecObject[j]->GetName())
				{
					vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 50.f + collOffset));
				}

				else if (L"Item Object" == vecObject[j]->GetName())
				{
					if (vecObject[j]->GetScript<CItemScript>()->getState() == ItemState::I_HpItem)
						vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, collOffset));

					else
						vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, collOffset, 0.f));
				}

				else if (L"Boss Object" == vecObject[j]->GetName())
				{
					vecObject[j]->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 50.f + collOffset));
				}
			}
		}
	}

	updateUI();
}

void CSceneMgr::update_tool()
{
	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();
	m_pCurScene->finalupdate();
}

void CSceneMgr::updateUI()
{
	// Transform 설정
	tResolution res = CRenderMgr::GetInst()->GetResolution();
	//Vec3 vScale(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.f);

	if (SceneState == SCENE_STATE::GAME_SCENE)
	{
		for (int i = 0; i < MAX_LAYER; ++i)
		{
			const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				// 미니맵에 플레이어 위치 업데이트
				if (L"PlayerPosUI" == vecObject[j]->GetName())
				{
					if (!m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->isDisappear)
					{
						Vec3 playerPos = m_pPlayerArr[playerID]->Transform()->GetLocalPos();
						playerPos = Vec3(playerPos.x + 5000.f, 0.f, playerPos.z + 5000.f);

						Vec2 posRatio = Vec2(playerPos.x / 10000.f * ((res.fWidth / miniMapUIRatio)), playerPos.z / 10000.f * ((res.fHeight / miniMapUIRatio) * (res.fWidth / res.fHeight)));

						Vec3 size = vecObject[j]->Transform()->GetLocalScale();

						Vec3 p1 = vecObject[j]->Transform()->GetLocalPos();
						Vec3 p2 = Vec3(-(res.fWidth / 2.f) + res.fWidth * 0.015 + posRatio.x
							, -(res.fHeight / 2.f) + res.fHeight * 0.015 + posRatio.y
							, 1.0f);

						vecObject[j]->Transform()->SetLocalPos(Vec3(p1.x * (1 - 0.3) + p2.x * 0.3, p1.y * (1 - 0.3) + p2.y * 0.3, 1.1f));
					}
					else
					{
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000.0f, -20000.0f, -3.f));
					}
				}

				//퀘스쳔 UI 화면 표시 유무
				else if (L"QuestUI" == vecObject[j]->GetName() && pressI)
				{
					if (QuestVisible)
						vecObject[j]->Transform()->SetLocalPos(Vec3(0, res.fHeight / 8, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(20000, 20000, 1.f));
				}

				//Hp 업데이트
				else if (L"HpBarUI" == vecObject[j]->GetName())
				{
					float hp = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->hp;

					vecObject[j]->Transform()->SetLocalScale(Vec3(hpBarWidth * hp / 100, hpBarHeight, 1.f));

					float adjust = 100 - hp;
					vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / 20 - adjust * 1.57, -res.fHeight / 2.75, 1.f));
				}

				//공격버프 UI 업데이트
				else if (L"PowerBuffOnUI" == vecObject[j]->GetName())
				{
					float powerBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->powerBuffTime;
					if (powerBuffTime > 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / 6.5, -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				else if (L"PowerBuffOffUI" == vecObject[j]->GetName())
				{
					float powerBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->powerBuffTime;
					if (powerBuffTime <= 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / 6.5, -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				//방어버프 UI 업데이트
				else if (L"DefenceBuffOnUI" == vecObject[j]->GetName())
				{
					float defenceBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->defenceBuffTime;
					if (defenceBuffTime > 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / 10, -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				else if (L"DefenceBuffOffUI" == vecObject[j]->GetName())
				{
					float defenceBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->defenceBuffTime;
					if (defenceBuffTime <= 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / 10, -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				//속도버프 UI 업데이트
				else if (L"SpeedBuffOnUI" == vecObject[j]->GetName())
				{
					float speedBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->speedBuffTime;
					if (speedBuffTime > 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / (130 / 6), -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				else if (L"SpeedBuffOffUI" == vecObject[j]->GetName())
				{
					float speedBuffTime = m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->speedBuffTime;
					if (speedBuffTime <= 0)
						vecObject[j]->Transform()->SetLocalPos(Vec3(-res.fWidth / (130 / 6), -res.fHeight / 2.3, 1.f));

					else
						vecObject[j]->Transform()->SetLocalPos(Vec3(-20000, 0.f, 1.f));
				}

				// 총알 갯수 UI
				else if (L"BulletCnt_UI" == vecObject[j]->GetName())
				{
					if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt <= 0)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[0].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 1)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[1].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 2)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[2].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 3)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[3].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 4)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[4].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 5)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[5].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 6)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[6].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 7)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[7].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 8)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[8].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 9)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[9].GetPointer());
					else if (m_pPlayerArr[playerID]->GetScript<CPlayerScript>()->GetStatus()->specialBulletCnt == 10)
						vecObject[j]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, BulletCntArrTex[10].GetPointer());
				}

			}
		}
	}

	pressI = false;
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

void CSceneMgr::setMap()
{
	// =================================
	// Map 생성
	// =================================
	CGameObject* pObject = nullptr;
	Ptr<CMeshData> pMeshData;
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Tomb2.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	//울타리 생성
	for (int i = 1; i < 260; i++)
	{
		if (((i <= 30 || i >= 35) && i >= 1 && i < 65) || ((i <= 95 || i >= 100) && i >= 65 && i < 130) || ((i <= 159 || i >= 165) && i >= 130 && i < 195) || ((i <= 224 || i >= 230) && i >= 195 && i < 260))
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
			pObject->MeshRender()->SetDynamicShadow(true);

			m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
		}
	}

	//무덤 생성
	for (int i = 0; i < 4; i++)
	{
		pObject = new CGameObject;
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Tomb2.mdat", L"MeshData\\Tomb2.mdat");
		pObject = pMeshData->Instantiate();
		pObject->SetName(L"Tomb Object");
		pObject->FrustumCheck(false);
		pObject->AddComponent(new CTransform);

		if (i == 0)
		{
			pObject->Transform()->SetLocalPos(Vec3(-5250.f, 0.f, 0.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
		}
		else if (i == 1)
		{
			pObject->Transform()->SetLocalPos(Vec3(5250.f, 0.f, 0.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, XM_PI / 2, 0.f));
		}
		else if (i == 2)
		{
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, -5250.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, XM_PI, 0.f));
		}
		else
		{
			pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 5250.f));
			pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, 0.f, 0.f));
		}

		pObject->Transform()->SetLocalScale(Vec3(2.0f, 1.5f, 2.0f));

		pObject->MeshRender()->SetDynamicShadow(true);

		pObject->AddComponent(new CCollider2D);
		pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 50.f + collOffset));
		pObject->Collider2D()->SetOffsetScale(Vec3(300.f, 300.f, 0.f));
		pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RRECT);

		pObject->AddComponent(new CTombScript());

		m_pCurScene->FindLayer(L"Tomb")->AddGameObject(pObject);

		m_pGameManager->tombArrInit(i, pObject);
	}


	////그라운드 생성
	//for (int i = 0; i < 6; i++)
	//	for (int j = 0; j < 6; j++)
	//	{
	//		pObject = new CGameObject;
	//		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Ground.mdat", L"MeshData\\Ground.mdat");
	//		pObject = pMeshData->Instantiate();
	//		pObject->SetName(L"Ground");
	//		pObject->FrustumCheck(false);
	//		pObject->AddComponent(new CTransform);
	//		pObject->Transform()->(Vec3(-5000 + i * 2000, 0.f, -5000 + j * 2000));
	//		pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
	//		pObject->Transform()->SetLocalScale(Vec3(2.f, 2.f, 1.f));
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
					pObject->Transform()->SetLocalPos(Vec3(-4800.f + j * 200, -15.f, -200 + i * 200));
					pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
					pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					//pObject->MeshRender()->SetDynamicShadow(true);
					m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
				}
				else
				{
					pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path2.mdat", L"MeshData\\Path2.mdat");
					pObject = pMeshData->Instantiate();
					pObject->SetName(L"Path2");
					pObject->FrustumCheck(true);
					pObject->AddComponent(new CTransform);
					pObject->Transform()->SetLocalPos(Vec3(-4800.f + j * 200, -15.f, -200 + i * 200));
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
						pObject->Transform()->SetLocalPos(Vec3(-200 + (i - 3) * 200, -15.f, -4800.f + j * 200));
						pObject->Transform()->SetLocalRot(Vec3(-XM_PI / 2, -XM_PI / 2, 0.f));
						pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
						//pObject->MeshRender()->SetDynamicShadow(true);
						m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
					}
					else
					{
						pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Path2.mdat", L"MeshData\\Path2.mdat");
						pObject = pMeshData->Instantiate();
						pObject->SetName(L"Path2");
						pObject->FrustumCheck(true);
						pObject->AddComponent(new CTransform);
						pObject->Transform()->SetLocalPos(Vec3(-200 + (i - 3) * 200, -15.f, -4800.f + j * 200));
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

void CSceneMgr::initValue()
{
	collOffset = 20000.f;
	QuestVisible = true;
	pressI = false;

	updateUI();
}