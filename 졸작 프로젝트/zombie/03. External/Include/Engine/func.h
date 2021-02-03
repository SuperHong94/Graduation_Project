#pragma once

void TestInit();

void TestUpdate();

void TestRender();

void TestRelease();


template<typename T, int iSize>
void Safe_Delete_Array(T(&_arr)[iSize])
{
	for (UINT i = 0; i < iSize; ++i)
	{
		SAFE_DELETE(_arr[i]);
	}
}

int GetSizeofFormat(DXGI_FORMAT _eFormat);