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

CCore::CCore()
	: m_hMainHwnd(nullptr)
{
}

CCore::~CCore()
{
}

int CCore::init(HWND _hWnd, const tResolution & _resolution, bool _bWindow)
{
	m_hMainHwnd = _hWnd;
	ChangeWindowSize(m_hMainHwnd, _resolution);
	ShowWindow(_hWnd, true);
<<<<<<< HEAD

=======
	CNetworkMgr::GetInst()->init();
>>>>>>> parent of 0651e18 (클라이언트 배열추가했더니 오류가남)
	if(FAILED(CDevice::GetInst()->init(_hWnd, _resolution, _bWindow)))
	{
		return E_FAIL;
	}

	CRenderMgr::GetInst()->init(_hWnd, _resolution, _bWindow);

	// ������� �����
	CDevice::GetInst()->CreateConstBuffer(L"TRANSFORM_MATRIX", sizeof(tTransform), 512, CONST_REGISTER::b0);
	CDevice::GetInst()->CreateConstBuffer(L"MATERIAL_PARAM", sizeof(tMtrlParam), 512, CONST_REGISTER::b1);
	CDevice::GetInst()->CreateConstBuffer(L"ANIM2D", sizeof(tMtrlParam), 512, CONST_REGISTER::b2);
		
	// ���� ������� ����(1������ ���� �������Ϳ��� ���ӵǾ���)
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT2D", sizeof(tLight2DInfo), 1, CONST_REGISTER::b3, true);
	CDevice::GetInst()->CreateConstBuffer(L"LIGHT3D", sizeof(tLight3DInfo), 1, CONST_REGISTER::b4, true);		
	CDevice::GetInst()->CreateConstBuffer(L"GLOBAL VALUE", sizeof(tGlobalValue), 1, CONST_REGISTER::b5);

	// ���� ������� ���
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b3), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b4), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b5), 0);
	
	// InstancingBuffer �ʱ�ȭ
	CInstancingMgr::GetInst()->init();
	
	// Manager �ʱ�ȭ
	CPathMgr::init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CSceneMgr::GetInst()->init();
	

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
<<<<<<< HEAD
=======
	CNetworkMgr::GetInst()->client_main();
>>>>>>> parent of 0651e18 (클라이언트 배열추가했더니 오류가남)
	CKeyMgr::GetInst()->update();
	CTimeMgr::GetInst()->update();
	CSound::g_pFMOD->update();

	CEventMgr::GetInst()->clear();
	{
		CSceneMgr::GetInst()->update();
		if(!CSceneMgr::GetInst()->CheckIsChange())
			CRenderMgr::GetInst()->render();

		CSceneMgr::GetInst()->SetIsChange(false);
	}
	CEventMgr::GetInst()->update();

	// �ν��Ͻ� ���� Ŭ����
	CInstancingMgr::GetInst()->clear();
}