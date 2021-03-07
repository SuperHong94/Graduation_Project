#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"


class CMeshRender :
	public CComponent
{
private:
	Ptr<CMesh>				m_pMesh;
	vector<Ptr<CMaterial>>  m_vecMtrl;	

public:
	Ptr<CMesh> GetMesh() { return m_pMesh; }
	void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }	

	Ptr<CMaterial> GetCloneMaterial(UINT _iSubset = 0);
	Ptr<CMaterial> GetSharedMaterial(UINT _iSubSet = 0) { return m_vecMtrl[_iSubSet]; }
	void SetMaterial(Ptr<CMaterial> _pMtrl, UINT _iSubset = 0);
	

public:
	void render();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

	CLONE(CMeshRender);
public:
	CMeshRender();
	virtual ~CMeshRender();
};

