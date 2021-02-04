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
	Ptr<CMesh>		m_pMesh;	
	Ptr<CMaterial>	m_pMtrl;

public:
	Ptr<CMesh> GetMesh() { return m_pMesh; }
	void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }	

	Ptr<CMaterial> GetCloneMaterial();
	Ptr<CMaterial> GetSharedMaterial() { return m_pMtrl; }

	void SetMaterial(Ptr<CMaterial> _pMtrl) { m_pMtrl = _pMtrl; }

public:
	void render();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

	CLONE(CMeshRender);
public:
	CMeshRender();
	virtual ~CMeshRender();
};

