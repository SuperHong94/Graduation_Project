#pragma once
#include "Entity.h"


class CResource :
	public CEntity
{
private:
	wstring		m_strPath;	// 상대경로
	int			m_iRefCount;
	RES_TYPE	m_eType;

	UINT		m_iCloneCount; // 리소스 복사 회수

public:
	void SetPath(const wstring& _strPath) { m_strPath = _strPath; }
	const wstring& GetPath() { return m_strPath; }

	RES_TYPE GetType() { return m_eType; }
	UINT GetCloneCount() { return m_iCloneCount; }

public:
	void AddRef() { m_iRefCount++; }
	void SubRef() { m_iRefCount--; }

public:
	virtual void Load(const wstring& _strPath/*최종 경로*/) {};
	virtual void Save(const wstring& _strPath/*최종 경로*/) {};

public:
	virtual CResource* Clone() { return nullptr; }
	void SaveToScene(FILE* _pFile);

	template<typename T>
	bool LoadFromScene(FILE* _pFile); // return false : 이미 리소스가 로딩이 되어있는 경우

public:
	CResource(RES_TYPE _eType);
	CResource(const CResource & _res);
	virtual ~CResource();

	template<typename T>
	friend class Ptr;
};

