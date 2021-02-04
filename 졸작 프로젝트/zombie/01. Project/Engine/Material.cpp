#include "stdafx.h"
#include "Material.h"


#include "Device.h"
#include "ConstantBuffer.h"
#include "ResMgr.h"
#include "PathMgr.h"

#include "Core.h"

CMaterial::CMaterial()
	: CResource(RES_TYPE::MATERIAL)
	, m_bFileSave(true)
{
	m_tParam.m_vDiff = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
	m_tParam.m_vSpec = Vec4(1.f, 1.f, 1.f, 1.f);
	m_tParam.m_vEmv = Vec4(0.f, 0.f, 0.f, 0.f);	
}

CMaterial::~CMaterial()
{
}


void CMaterial::SetShader(Ptr<CShader> _pShader)
{
	m_pShader = _pShader;
}

void CMaterial::SetData(SHADER_PARAM _eParam, void * _pData)
{
	switch (_eParam)
	{
	case SHADER_PARAM::INT_0:		
	case SHADER_PARAM::INT_1:		
	case SHADER_PARAM::INT_2:	
	case SHADER_PARAM::INT_3:
		m_tParam.m_arrInt[(UINT)_eParam - (UINT)SHADER_PARAM::INT_0] = *((int*)_pData);
		break;
	case SHADER_PARAM::FLOAT_0:
	case SHADER_PARAM::FLOAT_1:
	case SHADER_PARAM::FLOAT_2:
	case SHADER_PARAM::FLOAT_3:
		m_tParam.m_arrFloat[(UINT)_eParam - (UINT)SHADER_PARAM::FLOAT_0] = *((float*)_pData);
		break;
	case SHADER_PARAM::VEC2_0:
	case SHADER_PARAM::VEC2_1:
	case SHADER_PARAM::VEC2_2:
	case SHADER_PARAM::VEC2_3:
		m_tParam.m_arrVec2[(UINT)_eParam - (UINT)SHADER_PARAM::VEC2_0] = *((Vec2*)_pData);
		break;
	case SHADER_PARAM::VEC4_0:
	case SHADER_PARAM::VEC4_1:
	case SHADER_PARAM::VEC4_2:
	case SHADER_PARAM::VEC4_3:
		m_tParam.m_arrVec4[(UINT)_eParam - (UINT)SHADER_PARAM::VEC4_0] = *((Vec4*)_pData);
		break;
	case SHADER_PARAM::MATRIX_0:
	case SHADER_PARAM::MATRIX_1:
	case SHADER_PARAM::MATRIX_2:
	case SHADER_PARAM::MATRIX_3:
		m_tParam.m_arrMat[(UINT)_eParam - (UINT)SHADER_PARAM::MATRIX_0] = *((Matrix*)_pData);
		break;

	case SHADER_PARAM::TEX_0:
	case SHADER_PARAM::TEX_1:
	case SHADER_PARAM::TEX_2:
	case SHADER_PARAM::TEX_3:
		m_arrTex[(UINT)_eParam - (UINT)SHADER_PARAM::TEX_0] = (CTexture*)_pData;
		break;
	default:
		break;
	}

	//if (SCENE_MOD::SCENE_STOP == CCore::GetInst()->GetSceneMod())
	//{
	//	Save(CPathMgr::GetResPath());
	//}
}

void CMaterial::UpdateData()
{
	// Texture Register Update
	UINT iOffsetPos = (UINT)TEXTURE_REGISTER::t0;

	for (UINT i = 0; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			CDevice::GetInst()->SetTextureToRegister(m_arrTex[i].GetPointer(), TEXTURE_REGISTER(i + iOffsetPos));
			m_tParam.m_iArrTex[i] = 1;
		}
		else
		{			
			m_tParam.m_iArrTex[i] = 0;
		}
	}

	static CConstantBuffer* pCB = CDevice::GetInst()->GetCB(CONST_REGISTER::b1);
	CDevice::GetInst()->SetConstBufferToRegister(pCB, pCB->AddData(&m_tParam));	

	m_pShader->UpdateData();	
}

void CMaterial::Load(const wstring & _strFullPath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFullPath.c_str(), L"rb");

	// 쉐이더 키값
	wstring strName;
	strName = LoadWString(pFile);
	m_pShader = CResMgr::GetInst()->FindRes<CShader>(strName);

	// 쉐이더 파라미터
	fread(&m_tParam, sizeof(tMtrlParam), 1, pFile);

	UINT iMaxCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;
	for (UINT i = 0; i < iMaxCount; ++i)
	{
		UINT iExist = 0;
		fread(&iExist, 4, 1, pFile);

		if (iExist)
		{
			wstring strKey = LoadWString(pFile);
			wstring strPath = LoadWString(pFile);

			m_arrTex[i] = CResMgr::GetInst()->FindRes<CTexture>(strKey);
			if (nullptr == m_arrTex[i])
			{
				m_arrTex[i] = CResMgr::GetInst()->Load<CTexture>(strKey, strPath);
			}
		}
	}


	fclose(pFile);
}

void CMaterial::Save(const wstring & _strPath)
{
	if (!m_bFileSave)
		return;

	wstring strFilePath = _strPath;
	strFilePath += GetName();

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	// 쉐이더 키값
	SaveWString(pFile, m_pShader->GetName());

	// 쉐이더 파라미터
	fwrite(&m_tParam, sizeof(tMtrlParam), 1, pFile);
	
	UINT iMaxCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;
	for (UINT i = 0; i < iMaxCount; ++i)
	{
		UINT iExist = 1;
		if (nullptr == m_arrTex[i])
		{
			iExist = 0;
			fwrite(&iExist, 4, 1, pFile);
			continue;
		}
		
		fwrite(&iExist, 4, 1, pFile);
		SaveWString(pFile, m_arrTex[i]->GetName());
		SaveWString(pFile, m_arrTex[i]->GetPath());
	}

	fclose(pFile);
}

CMaterial * CMaterial::Clone()
{
	CResource::Clone();

	CMaterial* pClone = new CMaterial(*this);

	wchar_t szAdd[50] = {};
	wsprintf(szAdd, L"_Clone_%d.mtrl", GetCloneCount());

	// Naming
	wstring strPath = L"Material\\"; 
	strPath += CPathMgr::GetFileName(GetName().c_str());
	//strPath += szAdd;

	pClone->SetName(strPath);
	pClone->SetPath(strPath);
		
	CResMgr::GetInst()->AddCloneRes<CMaterial>(pClone);

	//if (SCENE_MOD::SCENE_STOP == CCore::GetInst()->GetSceneMod())
	//{	
	//	pClone->m_bFileSave = true;
	//	pClone->Save(CPathMgr::GetResPath());
	//}

	return pClone;
}
