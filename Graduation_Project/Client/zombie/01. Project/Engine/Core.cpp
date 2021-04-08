#include "stdafx.h"
#include "Core.h"

#include "Device.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ResMgr.h"

#include "SceneMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"

#include "PathMgr.h"
#include "ConstantBuffer.h"
#include "InstancingMgr.h"

#include "NetworkMgr.h"
CCore::CCore()
	: m_hMainHwnd(nullptr)
{
}

CCore::~CCore()
{
}

int CCore::init(HWND _hWnd, const tResolution & _resolution, bool _bWindow)
{
	std::wcout.imbue(std::locale("korean"));//오류코드 한국어로 표현


	m_hMainHwnd = _hWnd;
	ChangeWindowSize(m_hMainHwnd, _resolution);
	ShowWindow(_hWnd, true);

	if(FAILED(CDevice::GetInst()->init(_hWnd, _resolution, _bWindow)))
	{
		return E_FAIL;
	}

	CRenderMgr::GetInst()->init(_hWnd, _resolution, _bWindow);

	// 상수버퍼 만들기
	CDevice::GetInst()->CreateConstBuffer(L"TRANSFORM_MATRIX", sizeof(tTransform), 512, CONST_REGISTER::b0);
	CDevice::GetInst()->CreateConstBuffer(L"MATERIAL_PARAM", sizeof(tMtrlParam), 512, CONST_REGISTER::b1);
	CDevice::GetInst()->CreateConstBuffer(L"ANIM2D", sizeof(tMtrlParam), 512, CONST_REGISTER::b2);
		
	// 전역 상수버퍼 변수(1프레임 동안 레지스터에서 지속되야함)
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT2D", sizeof(tLight2DInfo), 1, CONST_REGISTER::b3, true);
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT3D", sizeof(tLight3DInfo), 1, CONST_REGISTER::b4, true);		
	CDevice::GetInst()->CreateConstBuffer(L"GLOBAL VALUE", sizeof(tGlobalValue), 1, CONST_REGISTER::b5);

	// 전역 상수버퍼 등록
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b3), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b4), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b5), 0);
	
	// InstancingBuffer 초기화
	CInstancingMgr::GetInst()->init();
	
	// Manager 초기화
	CPathMgr::init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CSceneMgr::GetInst()->init();
	
	//우선은 여기다하는데 나중에는 가장먼저로 올려야 함
	CNetworkMgr::GetInst()->init();
	return S_OK;
}

void CCore::ChangeWindowSize(HWND _hWnd, const tResolution & _resolution)
{
	RECT rt = { 0, 0, (int)_resolution.fWidth, (int)_resolution.fHeight };

	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
}

void CCore::progress()
{
	CNetworkMgr::GetInst()->client_main();
	CKeyMgr::GetInst()->update();
	CTimeMgr::GetInst()->update();
	CSound::g_pFMOD->update();
	CEventMgr::GetInst()->clear();
	{
		CSceneMgr::GetInst()->update();
		CRenderMgr::GetInst()->render();
	}
	CEventMgr::GetInst()->update();

	// 인스턴싱 버퍼 클리어
	CInstancingMgr::GetInst()->clear();
}