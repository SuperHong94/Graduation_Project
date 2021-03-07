#pragma once


class CCore
{
	SINGLE(CCore);
public:
	HWND	m_hMainHwnd;

public:
	int init(HWND _hWnd, const tResolution& _resolution, bool _bWindow);
	void progress();

private:
	void ChangeWindowSize(HWND _hWnd, const tResolution& _resolution);
};

