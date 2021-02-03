#pragma once
#include "Entity.h"

class CComponent;
class CTransform;
class CMeshRender;

class CGameObject :
	public CEntity
{
private:
	CComponent*	m_arrCom[(UINT)COMPONENT_TYPE::END];

public:
	void AddComponent(CComponent* _pCom);
	CComponent* GetComponent(COMPONENT_TYPE _eType) { return m_arrCom[(UINT)_eType]; }
	CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
	CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
	
public:
	void awake();
	void start();
	void update();
	void lateupdate();
	void finalupdate();

	void render();

	
	//template<typename T>
	//void AddComponent(T* _pCom);

public:
	CGameObject();
	virtual ~CGameObject();
};

//template<typename T>
//inline void CGameObject::AddComponent(T * _pCom)
//{
//	const type_info& info = typeid(T);
//
//	if (info.hash_code() == typeid(CTransform).hash_code()
//	{
//		m_arrCom[(UINT)COMPONENT_TYPE::T]
//	}
//}