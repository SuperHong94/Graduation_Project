#pragma once
#include "Resource.h"

#include "Ptr.h"
#include "Shader.h"
#include "Texture.h"

class CMaterial :
	public CResource
{
private:
	Ptr<CShader>	m_pShader;
	tMtrlParam      m_tParam;
	Ptr<CTexture>   m_arrTex[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0];

	bool			m_bFileSave;

public:
	void SetShader(Ptr<CShader> _pShader);
	void SetData(SHADER_PARAM _eParam, void* _pData);
	void DisableFileSave() { m_bFileSave = false; };
	void UpdateData();
	void UpdateData_CS();
	void Dispatch(UINT _x, UINT _y, UINT _z);

	virtual void Load(const wstring& _strFullPath);
	virtual void Save(const wstring& _strPath);

	Ptr<CShader> GetShader() { return m_pShader; }

	virtual CMaterial* Clone();

public:
	CMaterial();
	virtual ~CMaterial();
};

