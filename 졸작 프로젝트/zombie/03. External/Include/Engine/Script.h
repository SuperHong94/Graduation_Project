#pragma once
#include "Component.h"

#include "Transform.h"
#include "MeshRender.h"
#include "Collider2D.h"
#include "Animator2D.h"
#include "Animation2D.h"

#include "EventMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "ResMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

enum class SCRIPT_TYPE
{
	BULLETSCRIPT,
	MONSTERSCRIPT,
	PLAYERSCRIPT,
	TESTSCRIPT,
	END,
};

class CScript :
	public CComponent
{
private:
	UINT		m_iScriptType;

public:
	UINT GetScriptType() { return m_iScriptType; }

public:
	virtual void update() = 0;

public:
	static void CreateObject(CGameObject* _pNewObject, int _iLayerIdx);
	static void CreateObject(CGameObject* _pNewObject, const wstring& _strLayerName);

	static void DeleteObject(CGameObject* _pDeleteObject);

	void AddChild(CGameObject* _pChildObject);
	static void AddChild(CGameObject* _pParent, CGameObject* _pChild);

	void ClearParent();
	static void ClearParent(CGameObject* _pTarget);

	void TransferLayer(const wstring& _strLayerName, bool _bMoveAll);
	void TransferLayer(int _iLayerIdx, bool _bMoveAll);
	static void TransferLayer(CGameObject* _pTarget, const wstring& _strLayerName, bool _bMoveAll);
	static void TransferLayer(CGameObject* _pTarget, int _iLayerIdx, bool _bMoveAll);
	
	static void EableObject(CGameObject* _pTarget);
	static void Disable(CGameObject* _pTarget);

protected:
	virtual void OnCollisionEnter(CCollider2D* _pOther) {}
	virtual void OnCollision(CCollider2D* _pOther) {}
	virtual void OnCollisionExit(CCollider2D* _pOther) {}

public:
	virtual void SaveToScene(FILE* _pFile) {}
	virtual void LoadFromScene(FILE* _pFile) {}

	//virtual void OnCollisoinEnter(CCollider3D* _pOther) {}
	//virtual void OnCollisoin(CCollider3D* _pOther) {}
	//virtual void OnCollisoinExit(CCollider3D* _pOther) {}


public:
	CScript(UINT _iScriptType);
	virtual ~CScript();

	friend class CCollider2D;
	friend class CCOllider3D;
};

