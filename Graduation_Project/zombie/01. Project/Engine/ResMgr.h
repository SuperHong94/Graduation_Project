#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "Sound.h"
#include "MeshData.h"
#include "Ptr.h"

#include "PathMgr.h"

class CResMgr
{
	SINGLE(CResMgr);
private:
	map<wstring, CResource*> m_mapRes[(UINT)RES_TYPE::END];
	vector<CResource*> m_vecCloneRes[(UINT)RES_TYPE::END];

private:
	void CreateDefaultMesh();
	void CreateDefaultShader();
	void CreateDefaultMaterial();
	void InitSound();

public:
	void init();

	template<typename T>
	void AddRes(const wstring& _strKey, Ptr<T> _pRes);

	template<typename T>
	void AddCloneRes(Ptr<T> _pCloneRes);

	const map<wstring, CResource*>& GetResource(RES_TYPE _eType) { return m_mapRes[(UINT)_eType]; }

	template<typename T>
	Ptr<T> FindRes(const wstring& _strKey);

	template<typename T>
	Ptr<T> Load(const wstring& _strKey, const wstring& _strPath/*상대 경로*/);

	template<typename T>
	bool DestroyResource(const wstring & _strKey);

	Ptr<CTexture> CreateTexture(const wstring& _strName, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat
		, const D3D12_HEAP_PROPERTIES& _HeapProperty, D3D12_HEAP_FLAGS _eHeapFlag
		, D3D12_RESOURCE_FLAGS _eResFlag = D3D12_RESOURCE_FLAG_NONE, Vec4 _vClearColor = Vec4());

	Ptr<CTexture> CreateTextureFromResource(const wstring& _strName, ComPtr<ID3D12Resource> _pTex2D);

	// FBX
	Ptr<CMeshData> LoadFBX(const wstring & _strPath);
};


template<typename T>
RES_TYPE GetType()
{
	if (typeid(T).hash_code() == typeid(CMesh).hash_code())
		return RES_TYPE::MESH;
	if (typeid(T).hash_code() == typeid(CMeshData).hash_code())
		return RES_TYPE::MESHDATA;
	if (typeid(T).hash_code() == typeid(CMaterial).hash_code())
		return RES_TYPE::MATERIAL;
	if (typeid(T).hash_code() == typeid(CShader).hash_code())
		return RES_TYPE::SHADER;
	if (typeid(T).hash_code() == typeid(CTexture).hash_code())
		return RES_TYPE::TEXTURE;
	if (typeid(T).hash_code() == typeid(CSound).hash_code())
		return RES_TYPE::SOUND;
	return RES_TYPE::END;
}

template<typename T>
inline void CResMgr::AddRes(const wstring & _strKey, Ptr<T> _pRes)
{
	Ptr<T> pRes = FindRes<T>(_strKey);

	// 중복키 문제
	if (nullptr != pRes)
		assert(nullptr);

	RES_TYPE eType = GetType<T>();

	CResource** ppRes = (CResource**)&_pRes;
	m_mapRes[(UINT)eType].insert(make_pair(_strKey, *ppRes));
	_pRes->SetName(_strKey);
}


template<typename T>
void CResMgr::AddCloneRes(Ptr<T> _pCloneRes)
{
	assert(nullptr != _pCloneRes);

	RES_TYPE eType = GetType<T>();

	CResource** ppRes = (CResource**)&_pCloneRes;
	m_vecCloneRes[(UINT)eType].push_back(*ppRes);
}

template<typename T>
inline Ptr<T> CResMgr::FindRes(const wstring & _strKey)
{
	static map<wstring, CResource*>::iterator iter;

	RES_TYPE eType = GetType<T>();

	iter = m_mapRes[(UINT)eType].find(_strKey);

	if (iter == m_mapRes[(UINT)eType].end())
	{
		return nullptr;
	}

	return (T*)iter->second;
}

template<typename T>
inline Ptr<T> CResMgr::Load(const wstring & _strKey, const wstring & _strPath)
{
	Ptr<T> pRes = FindRes<T>(_strKey);

	// 중복키 문제
	if (nullptr != pRes)
		return pRes;

	pRes = new T;
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;
	pRes->Load(strFullPath);


	RES_TYPE eType = GetType<T>();

	CResource** ppRes = (CResource**)&pRes;
	m_mapRes[(UINT)eType].insert(make_pair(_strKey, *ppRes));
	pRes->SetName(_strKey);
	pRes->SetPath(_strPath);

	return pRes;
}

template<typename T>
inline bool CResMgr::DestroyResource(const wstring & _strKey)
{
	static map<wstring, CResource*>::iterator iter;

	RES_TYPE eType = GetType<T>();

	iter = m_mapRes[(UINT)eType].find(_strKey);

	if (iter == m_mapRes[(UINT)eType].end())
	{
		return false;
	}

	CResource* pRes = iter->second;
	SAFE_DELETE(pRes);
	m_mapRes[(UINT)eType].erase(iter);

	return true;
}