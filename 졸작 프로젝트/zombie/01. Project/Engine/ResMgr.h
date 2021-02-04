#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "Sound.h"
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

	// Texture
	//Ptr<CTexture> CreateTexture(const wstring _strKey, float _fWidth, float _iHeight
	//	, UINT _iBindFlag, DXGI_FORMAT _eFormat, D3D11_USAGE _eUsage = D3D11_USAGE_DEFAULT);
	//
	//Ptr<CTexture> CreateTexture(const wstring& _strKey, ID3D11Texture2D* _pTex2D);
};

template<typename T>
inline void CResMgr::AddRes(const wstring & _strKey, Ptr<T> _pRes)
{
	static const type_info& mesh = typeid(CMesh);
	static const type_info& shader = typeid(CShader);
	static const type_info& material = typeid(CMaterial);
	static const type_info& texture = typeid(CTexture);
	static const type_info& sound = typeid(CSound);
	   
	static RES_TYPE eType = RES_TYPE::END;

	Ptr<T> pRes = FindRes<T>(_strKey);
	
	// 중복키 문제
	if (nullptr != pRes)
		assert(nullptr);

	if (typeid(T).hash_code() == mesh.hash_code())
		eType = RES_TYPE::MESH;	
	else if (typeid(T).hash_code() == shader.hash_code())
		eType = RES_TYPE::SHADER;
	else if (typeid(T).hash_code() == material.hash_code())
		eType = RES_TYPE::MATERIAL;
	else if (typeid(T).hash_code() == texture.hash_code())
		eType = RES_TYPE::TEXTURE;
	else if (typeid(T).hash_code() == sound.hash_code())
		eType = RES_TYPE::SOUND;


	CResource** ppRes = (CResource**)&_pRes;
	m_mapRes[(UINT)eType].insert(make_pair(_strKey, *ppRes));
	_pRes->SetName(_strKey);
}

template<typename T>
void CResMgr::AddCloneRes(Ptr<T> _pCloneRes)
{
	static const type_info& mesh = typeid(CMesh);
	static const type_info& shader = typeid(CShader);
	static const type_info& material = typeid(CMaterial);
	static const type_info& texture = typeid(CTexture);
	static const type_info& sound = typeid(CSound);
	static RES_TYPE eType = RES_TYPE::END;	

	assert(nullptr != _pCloneRes);
	
	if (typeid(T).hash_code() == mesh.hash_code())
		eType = RES_TYPE::MESH;
	else if (typeid(T).hash_code() == shader.hash_code())
		eType = RES_TYPE::SHADER;
	else if (typeid(T).hash_code() == material.hash_code())
		eType = RES_TYPE::MATERIAL;		
	else if (typeid(T).hash_code() == texture.hash_code())
		eType = RES_TYPE::TEXTURE;	   
	else if (typeid(T).hash_code() == sound.hash_code())
		eType = RES_TYPE::SOUND;

	CResource** ppRes = (CResource**)&_pCloneRes;
	m_vecCloneRes[(UINT)eType].push_back(*ppRes);
}

template<typename T>
inline Ptr<T> CResMgr::FindRes(const wstring & _strKey)
{
	static const type_info& mesh = typeid(CMesh);
	static const type_info& shader = typeid(CShader);
	static const type_info& material = typeid(CMaterial);
	static const type_info& texture = typeid(CTexture);
	static const type_info& sound = typeid(CSound);
	static map<wstring, CResource*>::iterator iter;
	static RES_TYPE eType = RES_TYPE::END;

	if (typeid(T).hash_code() == mesh.hash_code())
		eType = RES_TYPE::MESH;			
	else if (typeid(T).hash_code() == shader.hash_code())
		eType = RES_TYPE::SHADER;
	else if (typeid(T).hash_code() == material.hash_code())
		eType = RES_TYPE::MATERIAL;
	else if (typeid(T).hash_code() == texture.hash_code())
		eType = RES_TYPE::TEXTURE;
	else if (typeid(T).hash_code() == sound.hash_code())
		eType = RES_TYPE::SOUND;


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
	static const type_info& mesh = typeid(CMesh);
	static const type_info& shader = typeid(CShader);
	static const type_info& material = typeid(CMaterial);
	static const type_info& texture = typeid(CTexture);
	static const type_info& sound = typeid(CSound);

	static RES_TYPE eType = RES_TYPE::END;


	Ptr<T> pRes = FindRes<T>(_strKey);

	// 중복키 문제
	if (nullptr != pRes)
		return pRes;
	
	if (typeid(T).hash_code() == mesh.hash_code())
		eType = RES_TYPE::MESH;
	else if (typeid(T).hash_code() == shader.hash_code())
		eType = RES_TYPE::SHADER;
	else if (typeid(T).hash_code() == material.hash_code())
		eType = RES_TYPE::MATERIAL;
	else if (typeid(T).hash_code() == texture.hash_code())
		eType = RES_TYPE::TEXTURE;
	else if (typeid(T).hash_code() == sound.hash_code())
		eType = RES_TYPE::SOUND;

	pRes = new T;
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;
	pRes->Load(strFullPath);

	CResource** ppRes = (CResource**)&pRes;
	m_mapRes[(UINT)eType].insert(make_pair(_strKey, *ppRes));
	pRes->SetName(_strKey);
	pRes->SetPath(_strPath);

	return pRes;
}
