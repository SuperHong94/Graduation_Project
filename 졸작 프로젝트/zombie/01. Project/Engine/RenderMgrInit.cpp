#include "stdafx.h"
#include "RenderMgr.h"

#include "Device.h"
#include "ResMgr.h"

#include "Core.h"

void CRenderMgr::init(HWND _hWnd, const tResolution & _res, bool _bWindow)
{
	m_hWnd = _hWnd;
	m_tResolution = _res;
	m_bWindowed = _bWindow;
	
	CreateSamplerState();

	CreateBlendState();

	CreateDepthStencilState();
}


void CRenderMgr::CreateSamplerState()
{
	
}


void CRenderMgr::CreateBlendState()
{

}

void CRenderMgr::CreateDepthStencilState()
{
}

