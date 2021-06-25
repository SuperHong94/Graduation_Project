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
#include "NetworkMgr.h"

CCore::CCore()
	: m_hMainHwnd(nullptr)
{
}

CCore::~CCore()
{
}

int CCore::init(HWND _hWnd, const tResolution& _resolution, bool _bWindow)
{
	m_hMainHwnd = _hWnd;
	res = _resolution;
	bWindow = _bWindow;
	ChangeWindowSize(m_hMainHwnd, _resolution);
	ShowWindow(_hWnd, true);

	CNetworkMgr::GetInst()->init();
	if (FAILED(CDevice::GetInst()->init(_hWnd, _resolution, _bWindow)))
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


	CPathMgr::init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CSceneMgr::GetInst()->init();


	return S_OK;
}

void CCore::ChangeWindowSize(HWND _hWnd, const tResolution& _resolution)
{
	RECT rt = { 0, 0, (int)_resolution.fWidth, (int)_resolution.fHeight };

	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	RECT rect_desktop = { 0, };
	HWND hWnd_desktop = GetDesktopWindow();
	GetWindowRect(hWnd_desktop, &rect_desktop);

	RECT rect_console = { 0, };
	HWND hWnd_console = GetConsoleWindow();
	GetWindowRect(hWnd_console, &rect_console);

	// 중앙에 화면 출력
	SetWindowPos(_hWnd, nullptr, (rect_desktop.left + rect_desktop.right - rect_console.right + rect_console.left) / 2 - (rt.right - rt.left) / 2,
		(rect_desktop.top + rect_desktop.bottom - rect_console.bottom + rect_console.top) / 2 - (rt.bottom - rt.top) / 2, rt.right - rt.left, rt.bottom - rt.top, 0);

	//SetWindowPos(_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
}

void CCore::progress()
{
	if (KEY_TAB(KEY_TYPE::KEY_ESC))
	{
		ExitProcess(1);
	}

	CNetworkMgr::GetInst()->client_main();
	CKeyMgr::GetInst()->update();
	CTimeMgr::GetInst()->update();
	CSound::g_pFMOD->update();

	CEventMgr::GetInst()->clear();
	{
		CSceneMgr::GetInst()->update();
		if (!CSceneMgr::GetInst()->CheckIsChange())
			CRenderMgr::GetInst()->render();

		CSceneMgr::GetInst()->SetIsChange(false);
	}
	CEventMgr::GetInst()->update();
}