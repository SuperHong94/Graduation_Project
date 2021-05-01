#include "stdafx.h"
#include "Camera.h"

#include "Transform.h"
#include "Device.h"
#include "SceneMgr.h"
#include "RenderMgr.h"

#include "Scene.h"
#include "Layer.h"

#include "KeyMgr.h"
#include "TimeMgr.h"

#include "MeshRender.h"
#include "Collider2D.h"
#include "ParticleSystem.h"
#include "Animator3D.h"

#include "InstancingMgr.h"


CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_frustum(this)
	, m_fFar(1000.f)
	, m_fNear(1.f)
	, m_fFOV(XM_PI / 4.f)
	, m_fScale(1.f)
	, m_eProjType(PROJ_TYPE::PERSPECTIVE)
	, m_iLayerCheck(0)
	, m_bModule(false)
{		
}

CCamera::~CCamera()
{
}

void CCamera::finalupdate()
{
	// 뷰행렬
	Vec3 vPos = Transform()->GetWorldPos();
	Matrix matViewTrans = XMMatrixTranslation(-vPos.x, -vPos.y, -vPos.z);

	Matrix matViewRot = XMMatrixIdentity();
	Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vUp	= Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	matViewRot._11 = vRight.x; matViewRot._12 = vUp.x; matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y; matViewRot._22 = vUp.y; matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z; matViewRot._32 = vUp.z; matViewRot._33 = vFront.z;

	m_matView = matViewTrans * matViewRot;

	// 투영행렬
	m_matProj = XMMatrixIdentity();

	tResolution res = CRenderMgr::GetInst()->GetResolution();

	if (m_eProjType == PROJ_TYPE::PERSPECTIVE)
	{		
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, res.fWidth / res.fHeight, m_fNear, m_fFar);
	}
	else
	{
		m_matProj = XMMatrixOrthographicLH(m_fWidth * m_fScale, m_fHeight * m_fScale, m_fNear, m_fFar);
		//m_matProj = XMMatrixOrthographicOffCenterLH(0.f, res.fWidth, res.fHeight, 0.f, m_fNear, m_fFar);		
	}

	m_matViewInv = XMMatrixInverse(nullptr, m_matView);
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	m_frustum.finalupdate();

	// 모듈로 사용될때는 RenderMgr 에 등록하지 않는다
	if (!m_bModule)
		CRenderMgr::GetInst()->RegisterCamera(this);	
}

void CCamera::SortGameObject()
{
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_P)
		pair.second.clear();

	m_vecParticle.clear();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (m_iLayerCheck & (1 << i))
		{
			const vector<CGameObject*>& vecObj = pCurScene->GetLayer(i)->GetObjects();

			for (UINT i = 0; i < vecObj.size(); ++i)
			{
				if (!vecObj[i]->GetFrustumCheck()
					|| m_frustum.CheckFrustumSphere(vecObj[i]->Transform()->GetWorldPos(), vecObj[i]->Transform()->GetMaxScale()))
				{
					if (vecObj[i]->MeshRender() && vecObj[i]->MeshRender()->GetMesh() != nullptr)
					{
						UINT iMtrlCount = vecObj[i]->MeshRender()->GetMaterialCount();

						for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
						{
							if (vecObj[i]->MeshRender()->GetSharedMaterial(iMtrl) == nullptr
								|| vecObj[i]->MeshRender()->GetSharedMaterial(iMtrl)->GetShader() == nullptr)
							{
								if (vecObj[i]->Particlesystem())									
								{
									m_vecParticle.push_back(vecObj[i]);
								}
								continue;
							}

							Ptr<CMaterial> pMtrl = vecObj[i]->MeshRender()->GetSharedMaterial(iMtrl);

							// Material 을 참조하고 있지 않거나, Material 에 아직 Shader 를 연결하지 않은 상태라면 Continue
							if (nullptr == pMtrl || pMtrl->GetShader() == nullptr)
								continue;

							// Shader 가 Deferred 인지 Forward 인지에 따라서
							// 인스턴싱 그룹을 분류한다.
							map<ULONG64, vector<tInstObj>>* pMap = NULL;
							if (pMtrl->GetShader()->GetShaderPOV() == SHADER_POV::DEFERRED)
								pMap = &m_mapInstGroup_D;
							else if (pMtrl->GetShader()->GetShaderPOV() == SHADER_POV::FORWARD)
								pMap = &m_mapInstGroup_F;
							else if (pMtrl->GetShader()->GetShaderPOV() == SHADER_POV::POSTEFFECT)
								pMap = &m_mapInstGroup_P;
							else
								continue;

							uInstID uIntanceID = {};
							uIntanceID.llID = vecObj[i]->MeshRender()->GetInstID(iMtrl);
							map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uIntanceID.llID);
							if (iter == pMap->end())
							{
								pMap->insert(make_pair(uIntanceID.llID, vector<tInstObj>{tInstObj{ vecObj[i], iMtrl }}));
							}
							else
							{
								iter->second.push_back(tInstObj{ vecObj[i], iMtrl });
							}
						}
					}
				}
			}
		}
	}
}

void CCamera::SortShadowObject()
{
	m_vecShadowObj.clear();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = nullptr;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		pLayer = pCurScene->GetLayer(i);
		if (nullptr == pLayer || !(m_iLayerCheck & (1 << i)))
			continue;

		const vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			if (!vecObj[j]->GetFrustumCheck()
				|| m_frustum.CheckFrustumSphere(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetMaxScale()))
			{
				if (vecObj[j]->MeshRender()
					&& vecObj[j]->MeshRender()->GetMesh() != nullptr
					&& vecObj[j]->MeshRender()->GetSharedMaterial() != nullptr
					&& vecObj[j]->MeshRender()->GetSharedMaterial()->GetShader() != nullptr
					&& vecObj[j]->MeshRender()->IsDynamicShadow())
				{
					m_vecShadowObj.push_back(vecObj[j]);
				}
			}
		}
	}
}

void CCamera::render_deferred()
{
	g_transform.matView = GetViewMat();
	g_transform.matProj = GetProjMat();
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProjInv = m_matProjInv;
	
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;
		else if (pair.second.size() < INSTANCING_COUNT // instancing 개수 조건
			|| false == pair.second[0].pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx)->GetShader()->IsPossibleInstancing()) 
		{
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		if (nullptr == pMtrl->GetShader())
			continue;
		
		CInstancingBuffer* pInstBuffer = CInstancingMgr::GetInst()->GetInstancingBuffer((long long)pObj);
		if (pInstBuffer == nullptr)
		{
			// 인스턴싱 데이터를 모을 버퍼 할당
			pInstBuffer = CInstancingMgr::GetInst()->AllocBuffer((long long)pObj);
		}

		if (false == pInstBuffer->BeUpdated())
		{
			int iRowIdx = 0;
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				if (pair.second[i].pObj->Animator2D())//|| pair.second[i].pObj->Animator3D())
				{
					map<INT_PTR, vector<tInstObj>>::iterator iter
						= m_mapSingleObj.find((INT_PTR)pair.second.at(0).pObj);

					if (iter != m_mapSingleObj.end())
						iter->second.push_back(pair.second[i]);
					else
					{
						m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[i]}));
					}
					continue;
				}

				// 데이터를 모아서 인스턴싱 버퍼에 전달
				tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
				tInstData.matWV = tInstData.matWorld * m_matView;
				tInstData.matWVP = tInstData.matWV * m_matProj;

				if (pair.second[i].pObj->Animator3D())
				{
					pInstBuffer->Resize_BoneBuffer((UINT)pair.second.size(), sizeof(Matrix) * pMesh->GetBoneCount());
					pair.second[i].pObj->Animator3D()->UpdateData_Inst(pInstBuffer->GetBoneBuffer(), iRowIdx);
					tInstData.iRowIdx = iRowIdx++;					
					CInstancingMgr::GetInst()->AddInstancingData(tInstData, true);
				}
				else
				{
					tInstData.iRowIdx = -1;
					CInstancingMgr::GetInst()->AddInstancingData(tInstData, false);
				}
			}

			// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
			if(0 != pInstBuffer->GetInstanceCount())
				CInstancingMgr::GetInst()->SetData(); 
		}
		
		if (0 != pInstBuffer->GetInstanceCount())
		{
			if (pInstBuffer->GetAnimInstancingCount() > 0)
			{
				int iAnim = 1;
				int iBoneCount = pair.second[0].pObj->Animator3D()->GetBoneCount();
				pMtrl->SetData(SHADER_PARAM::INT_0, &iAnim);
				pMtrl->SetData(SHADER_PARAM::INT_1, &iBoneCount);
				pInstBuffer->GetBoneBuffer()->UpdateData(TEXTURE_REGISTER::t7);				
			}
					   
			pMtrl->UpdateData(1);
			pMesh->render_instancing(pair.second[0].iMtrlIdx, pInstBuffer);

			// Animatino 행렬 값 정리
			int a = 0;			
			pMtrl->SetData(SHADER_PARAM::INT_0, &a);
			pMtrl->SetData(SHADER_PARAM::INT_1, &a);
		}		
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		for (auto& tInstObj : pair.second)
		{
			tInstObj.pObj->MeshRender()->render(tInstObj.iMtrlIdx);
		}
	}
}

void CCamera::render_forward()
{
	g_transform.matView = GetViewMat();
	g_transform.matProj = GetProjMat();
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProjInv = m_matProjInv;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;
		else if (pair.second.size() < INSTANCING_COUNT // instancing 개수 조건
			|| false == pair.second[0].pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx)->GetShader()->IsPossibleInstancing())
		{
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		if (nullptr == pMtrl->GetShader())
			continue;

		CInstancingBuffer* pInstBuffer = CInstancingMgr::GetInst()->GetInstancingBuffer(pMesh->GetID());
		if (pInstBuffer == nullptr)
		{
			// 인스턴싱 데이터를 모을 버퍼 할당
			pInstBuffer = CInstancingMgr::GetInst()->AllocBuffer(pMesh->GetID());
		}

		if (false == pInstBuffer->BeUpdated())
		{
			int iRowIdx = 0;
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				if (pair.second[i].pObj->Animator2D())// || pair.second[i].pObj->Animator3D())
				{
					map<INT_PTR, vector<tInstObj>>::iterator iter
						= m_mapSingleObj.find((INT_PTR)pair.second.at(0).pObj);

					if (iter != m_mapSingleObj.end())
						iter->second.push_back(pair.second[i]);
					else
					{
						m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[i]}));
					}
					continue;
				}

				// 데이터를 모아서 인스턴싱 버퍼에 전달
				tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
				tInstData.matWV = tInstData.matWorld * m_matView;
				tInstData.matWVP = tInstData.matWV * m_matProj;

				if (pair.second[i].pObj->Animator3D())
				{
					pInstBuffer->Resize_BoneBuffer((UINT)pair.second.size(), sizeof(Matrix) * pMesh->GetBoneCount());
					pair.second[i].pObj->Animator3D()->UpdateData_Inst(pInstBuffer->GetBoneBuffer(), tInstData.iRowIdx);
					tInstData.iRowIdx = iRowIdx++;
					//CInstancingMgr::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
					CInstancingMgr::GetInst()->AddInstancingData(tInstData, true);
				}
				else
				{
					tInstData.iRowIdx = -1;
					CInstancingMgr::GetInst()->AddInstancingData(tInstData, false);
				}
			}

			// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
			if (0 != pInstBuffer->GetInstanceCount())
				CInstancingMgr::GetInst()->SetData();
		}

		if (0 != pInstBuffer->GetInstanceCount())
		{
			if (pInstBuffer->GetAnimInstancingCount() > 0)
			{
				int iAnim = 1;
				int iBoneCount = pair.second[0].pObj->Animator3D()->GetBoneCount();
				pMtrl->SetData(SHADER_PARAM::INT_0, &iAnim);
				pMtrl->SetData(SHADER_PARAM::INT_1, &iBoneCount);
				pInstBuffer->GetBoneBuffer()->UpdateData(TEXTURE_REGISTER::t7);
			}

			pMtrl->UpdateData(1);
			pMesh->render_instancing(pair.second[0].iMtrlIdx, pInstBuffer);

			// Animatino 행렬 값 정리
			int a = 0;
			pMtrl->SetData(SHADER_PARAM::INT_0, &a);
			pMtrl->SetData(SHADER_PARAM::INT_1, &a);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		for (auto& tInstObj : pair.second)
		{
			tInstObj.pObj->MeshRender()->render(tInstObj.iMtrlIdx);

			// 충돌체 보유 시, 충돌체도 그려준다.
			if (tInstObj.pObj->Collider2D())			
				tInstObj.pObj->Collider2D()->render();			
		}
	}
	
	// Particle Rendering
	for (size_t i = 0; i < m_vecParticle.size(); ++i)
	{
		m_vecParticle[i]->Particlesystem()->render();
	}
	
	// Deferred Collider rendering
	for (auto& pair : m_mapInstGroup_D)
	{
		for (size_t i = 0; i < pair.second.size(); ++i)
		{
			if (pair.second[i].pObj->Collider2D())
			{
				pair.second[i].pObj->Collider2D()->render();
			}
		}	
	}
}

void CCamera::render_posteffect()
{
	g_transform.matView = GetViewMat();
	g_transform.matProj = GetProjMat();
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProjInv = m_matProjInv;

	for (auto& pair : m_mapInstGroup_P)
	{
		for (size_t i = 0; i < pair.second.size(); ++i)
		{
			pair.second[i].pObj->MeshRender()->render(pair.second[i].iMtrlIdx);
		}
	}
}


void CCamera::render()
{
	g_transform.matView = GetViewMat();
	g_transform.matProj = GetProjMat();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{	
		if (m_iLayerCheck & (1 << i))
		{
			const vector<CGameObject*>& vecObj = pCurScene->GetLayer(i)->GetObjects();

			for (UINT i = 0; i < vecObj.size(); ++i)
			{
				if (!vecObj[i]->GetFrustumCheck() 
					|| m_frustum.CheckFrustumSphere(vecObj[i]->Transform()->GetWorldPos(), vecObj[i]->Transform()->GetMaxScale()))					
				{
					if (vecObj[i]->MeshRender())
					{
						vecObj[i]->MeshRender()->render();
					}

					if (vecObj[i]->Collider2D())
					{
						vecObj[i]->Collider2D()->render();
					}
				}
			}
		}
	}
}


void CCamera::render_shadowmap()
{
	// 뷰행렬과 투영행렬을 광원시점 카메라의 것으로 대체해둠
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;
	g_transform.matViewInv = m_matViewInv;

	for (UINT i = 0; i < m_vecShadowObj.size(); ++i)
	{
		m_vecShadowObj[i]->MeshRender()->render_shadowmap();
	}
}

void CCamera::SaveToScene(FILE * _pFile)
{
	UINT iType = (UINT)GetComponentType();
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_fNear, sizeof(float), 1, _pFile);
	fwrite(&m_fFar, sizeof(float), 1, _pFile);

	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_fScale, sizeof(float), 1, _pFile);
	
	fwrite(&m_eProjType, sizeof(UINT), 1, _pFile);
	fwrite(&m_iLayerCheck, 4, 1, _pFile);
}

void CCamera::LoadFromScene(FILE * _pFile)
{
	fread(&m_fNear, sizeof(float), 1, _pFile);
	fread(&m_fFar, sizeof(float), 1, _pFile);
	
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_fScale, sizeof(float), 1, _pFile);
	
	fread(&m_eProjType, sizeof(UINT), 1, _pFile);
	fread(&m_iLayerCheck, 4, 1, _pFile);
}
