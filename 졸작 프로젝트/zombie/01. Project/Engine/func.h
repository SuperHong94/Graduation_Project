#pragma once

void TestInit();

void TestUpdate();

void TestRender();

void TestRelease();


int GetSizeofFormat(DXGI_FORMAT _eFormat);

template<typename T1, typename T2>
void Safe_Delete_Map(map<T1, T2>& _map)
{
	for (auto& pair : _map)
	{
		SAFE_DELETE(pair.second);
	}

	_map.clear();
}

template<typename T, int iSize>
void Safe_Delete_Array(T*(&_arr)[iSize])
{
	for (UINT i = 0; i < iSize; ++i)
	{
		SAFE_DELETE(_arr[i]);
	}
}

template<typename T>
void Safe_Delete_Vector(vector<T*> _vec)
{
	for (UINT i = 0; i < _vec.size(); ++i)
	{
		SAFE_DELETE(_vec[i]);
	}
}

void SaveWString(FILE* _pFile, const wstring& _str);

wchar_t* LoadWString(FILE* _pFile);