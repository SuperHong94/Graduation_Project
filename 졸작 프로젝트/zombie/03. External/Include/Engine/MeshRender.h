#pragma once
#include "Component.h"

#include "Ptr.h"
#include "Mesh.h"
#include "Shader.h"


class CMeshRender :
	public CComponent
{
private:
	Ptr<CMesh>		m_pMesh;
	Ptr<CShader>	m_pShader;

public:
	void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }
	void SetShader(Ptr<CShader> _pShader) {m_pShader = _pShader;}

public:
	void render();

public:
	CMeshRender();
	virtual ~CMeshRender();
};

