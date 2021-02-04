#pragma once

class CBlendState;
class CDepthStencilState;
class CCamera;
class CLight3D;

class CRenderTarget24;
class CMRT;

#include "Ptr.h"
#include "Texture.h"

class CRenderMgr
{
	SINGLE(CRenderMgr);

private:
	IDXGISwapChain*			m_pSwapChain;
	Ptr<CTexture>			m_pRenderTargetTex;
	Ptr<CTexture>			m_pDepthStencilTex;
		
	tResolution				m_tResolution;

	CRenderTarget24*		m_arrRT[(UINT)RT_TYPE::END];
	CMRT*					m_arrMRT[(UINT)MRT_TYPE::END];

	CBlendState*			m_arrBlendState[(UINT)BLEND_TYPE::END];
	CDepthStencilState*		m_arrDepthStencilState[(UINT)BLEND_TYPE::END];

	tLight2DInfo			m_tLight2DInfo;

	vector<CLight3D*>		m_vecLight3D;
	vector<CCamera*>		m_vecCam;
	
	bool					m_bWindowed;

public:
	void init(HWND _hWnd, const tResolution & _res, bool _bWindow);
	void render();
	void render_tool();		
	void Present() { m_pSwapChain->Present(0, 0); }

private:
	void CreateSamplerState();
	void CreateBlendState();
	void CreateDepthStencilState();
	
	void UpdateLight2D();
	void UpdateLight3D();

public:
	CBlendState* GetBlendState(BLEND_TYPE _eType) { return m_arrBlendState[(UINT)_eType]; }
	CDepthStencilState* GetDepthStencilState(BLEND_TYPE _eType) { return m_arrDepthStencilState[(UINT)_eType]; }

	void RegisterLight2D(const tLight2D& _Light2D)
	{
		if (100 <= m_tLight2DInfo.iCount)
		{
			return;
		}

		m_tLight2DInfo.arrLight2D[m_tLight2DInfo.iCount++] = _Light2D;
	}
	void RegisterLight3D(CLight3D* _pLight3D) { if (m_vecLight3D.size() >= 100) return; m_vecLight3D.push_back(_pLight3D); }

	CCamera* GetCamera(int _iIdx) { return m_vecCam[_iIdx]; }
	void RegisterCamera(CCamera* _pCam) { m_vecCam.push_back(_pCam); }
	void ClearCamera() { m_vecCam.clear(); }

	tResolution GetResolution() { return m_tResolution; }

	friend class CSceneMgr;
};


