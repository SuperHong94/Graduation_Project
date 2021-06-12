#pragma once


class CCore
{
	SINGLE(CCore);
public:
	HWND	m_hMainHwnd;
	tResolution	res;
	bool bWindow;

public:
	int init(HWND _hWnd, const tResolution& _resolution, bool _bWindow);
	void progress();

private:
	void ChangeWindowSize(HWND _hWnd, const tResolution& _resolution);
};

