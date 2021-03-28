#include "stdafx.h"
#include "RenderMgr.h"

#include "Device.h"
#include "Camera.h"

#include "SceneMgr.h"
#include "ConstantBuffer.h"

#include "TimeMgr.h"
#include "EventMgr.h"
#include "ResMgr.h"

#include "Light3D.h"
#include "MRT.h"

CRenderMgr::CRenderMgr()
	: m_arrMRT{}		
	, m_iRTVHeapSize(0)	
{	
}

CRenderMgr::~CRenderMgr()
{	
	Safe_Delete_Array(m_arrMRT);
}

void CRenderMgr::render()
{
	// 초기화
	float arrColor[4] = { 0.f,0.f, 0.f, 1.f };
	CDevice::GetInst()->render_start(arrColor);

	// 전역버퍼 데이터 업데이트
	static CConstantBuffer* pGlobalBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b5);		
	CDevice::GetInst()->SetConstBufferToRegister(pGlobalBuffer, pGlobalBuffer->AddData(&g_global));

	// 광원 정보 업데이트
	UpdateLight2D();
	UpdateLight3D();

	// SwapChain MRT 초기화
	UINT iIdx = CDevice::GetInst()->GetSwapchainIdx();
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Clear(iIdx);

	// DeferredMRT 초기화
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Clear();

	// LightMRT 초기화
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Clear();

	
	// ==================================
	// Main Camera 로 Deferred 렌더링 진행
	// ==================================
	m_vecCam[0]->SortGameObject();
		
	// Deferred MRT 셋팅
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();
	m_vecCam[0]->render_deferred();
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->TargetToResBarrier();

	// shadowmap 만들기
	render_shadowmap();

	// Render Light
	render_lights();
		
	// Merge (Diffuse Target, Diffuse Light Target, Specular Target )		
	merge_light();

	// Forward Render
	m_vecCam[0]->render_forward(); // skybox, grid, ui

	// PostEffectRender
	m_vecCam[0]->render_posteffect();

	//=================================
	// 추가 카메라는 forward render 만
	//=================================
	for (int i = 1; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->SortGameObject();
		m_vecCam[i]->render_forward();
	}	

	// 출력
	CDevice::GetInst()->render_present();
}

void CRenderMgr::render_tool()
{
	// 초기화
	float arrColor[4] = { 0.f, 0.f, 0.f, 1.f };
	//Clear(arrColor);

	// 광원 정보 업데이트
	UpdateLight2D();
	UpdateLight3D();
}

void CRenderMgr::UpdateLight2D()
{
	static CConstantBuffer* pLight2DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b3);

	UINT iOffsetPos = pLight2DBuffer->AddData(&m_tLight2DInfo);
	CDevice::GetInst()->SetConstBufferToRegister(pLight2DBuffer, iOffsetPos);

	m_tLight2DInfo.iCount = 0;
}

void CRenderMgr::UpdateLight3D()
{
	static CConstantBuffer* pLight3DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b4);
	
	tLight3DInfo tLight3DArray;

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		const tLight3D& info = m_vecLight3D[i]->GetLight3DInfo();
		tLight3DArray.arrLight3D[i] = info;
	}
	tLight3DArray.iCurCount = (UINT)m_vecLight3D.size();
	
	UINT iOffsetPos = pLight3DBuffer->AddData(&tLight3DArray);
	CDevice::GetInst()->SetConstBufferToRegister(pLight3DBuffer, iOffsetPos);
}

CCamera * CRenderMgr::GetMainCam()
{
	/*if (CCore::GetInst()->GetSceneMod() == SCENE_MOD::SCENE_PLAY)
	{
		if (!m_vecCam.empty())
			return m_vecCam[0];
		return nullptr;
	}*/

	return  m_vecCam[0];
}

void CRenderMgr::CopySwapToPosteffect()
{
	static CTexture* pPostEffectTex = CResMgr::GetInst()->FindRes<CTexture>(L"PosteffectTargetTex").GetPointer();
	
	UINT iIdx = CDevice::GetInst()->GetSwapchainIdx();

	// SwapChain Target Texture 를 RenderTarget -> CopySource 상태로 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->GetRTTex(iIdx)->GetTex2D().Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);

	CMDLIST->ResourceBarrier(1, &resurceBarrier);

	// SwapChainTex -> PostEfectTex 로 복사
	CMDLIST->CopyResource(pPostEffectTex->GetTex2D().Get()
		, m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->GetRTTex(iIdx)->GetTex2D().Get());

	// SwapChain Target Texture 를 CopySource -> RenderTarget 상태로 변경
	CD3DX12_RESOURCE_BARRIER resurceBarrier2 = CD3DX12_RESOURCE_BARRIER::Transition(m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->GetRTTex(iIdx)->GetTex2D().Get()
		, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	CMDLIST->ResourceBarrier(1, &resurceBarrier2);
}


void CRenderMgr::render_shadowmap()
{	
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->Clear();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->OMSet();

	// 광원 시점으로 깊이를 그림
	for (UINT i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (m_vecLight3D[i]->GetLight3DInfo().iLightType != (UINT)LIGHT_TYPE::DIR)
			continue;

		m_vecLight3D[i]->render_shadowmap();
	}	

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->TargetToResBarrier();
}

void CRenderMgr::render_lights()
{
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();
		
	// 광원을 그린다.
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// 메인 카메라 시점 기준 View, Proj 행렬로 되돌린다.
	g_transform.matView = pMainCam->GetViewMat();
	g_transform.matProj = pMainCam->GetProjMat();
	g_transform.matViewInv = pMainCam->GetViewMatInv();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->Light3D()->render();
	}

	m_vecLight3D.clear();
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->TargetToResBarrier();

	// SwapChain MRT 셋팅
	UINT iIdx = CDevice::GetInst()->GetSwapchainIdx();
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet(1, iIdx);
}

void CRenderMgr::merge_light()
{
	// ========================
	// Merge (Diffuse + Lights)
	// ========================
	static Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	static Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");

	pMtrl->UpdateData();
	pRectMesh->render();
}
