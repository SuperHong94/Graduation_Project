#include "stdafx.h"
#include "Core.h"

#include "Device.h"
#include "KeyMgr.h"
#include "TimeMgr.h"

#include "PathMgr.h"

CCore::CCore()
	: m_hMainHwnd(nullptr)
{
}

CCore::~CCore()
{
	TestRelease();
}

int CCore::init(HWND _hWnd, const tResolution & _resolution, bool _bWindow)
{
	m_hMainHwnd = _hWnd;
	ChangeWindowSize(m_hMainHwnd, _resolution);
	ShowWindow(_hWnd, true);

	if(FAILED(CDevice::GetInst()->init(_hWnd, _resolution, _bWindow)))
	{
		return E_FAIL;
	}

	// 상수버퍼 만들기
	CDevice::GetInst()->CreateConstBuffer(L"GLOBAL_MATRIX_1", sizeof(tTransform), 512, CONST_REGISTER::b0);
	CDevice::GetInst()->CreateConstBuffer(L"GLOBAL_MATRIX_2", sizeof(tTransform), 512, CONST_REGISTER::b1);
		
	CPathMgr::init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();

	TestInit();

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
	CKeyMgr::GetInst()->update();
	CTimeMgr::GetInst()->update();
	   	 
	update();
	lateupdate();
	finalupdate();
	
	render();
}

void CCore::update()
{
	TestUpdate();
}

void CCore::lateupdate()
{
}

void CCore::finalupdate()
{
}

void CCore::render()
{
	TestRender();
}
