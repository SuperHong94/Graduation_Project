#include "stdafx.h"
#include "Animator2D.h"

#include "Animation2D.h"
#include "ConstantBuffer.h"
#include "Device.h"


CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
{
}

CAnimator2D::CAnimator2D(const CAnimator2D & _origin)
	: CComponent(_origin)
	, m_bRepeat(_origin.m_bRepeat)
{
	for (auto& pair : _origin.m_mapAnim)
	{			
		CAnimation2D* pCopyAnim = new CAnimation2D(*pair.second);
		pCopyAnim->SetAnimator2D(this);
		m_mapAnim.insert(make_pair(pair.first, pCopyAnim));
	}

	if (nullptr != _origin.m_pCurAnim)
	{
		PlayAnimation(_origin.m_pCurAnim->GetName(), m_bRepeat);
	}
}

CAnimator2D::~CAnimator2D()
{
	Safe_Delete_Map(m_mapAnim);
}


void CAnimator2D::finalupdate()
{
	if (!m_bPlay)
		return;

	if (nullptr != m_pCurAnim)
	{
		// 반복모드 이고 한번 재생이 끝에 도달하면
		if (m_pCurAnim->IsFinish())
		{
			if (m_bRepeat)
			{
				m_pCurAnim->Play();
			}
			else
			{
				m_bPlay = false;
			}
		}

		m_pCurAnim->finalupdate();
	}	
}

bool CAnimator2D::AddAnimation(const wstring & _strAnimKey
	, Ptr<CTexture> _pTex
	, Vec2 _vLT, Vec2 _vGap
	, int _iFrmCount, float _fDuration)
{
	CAnimation2D* pAnim = FindAnimation(_strAnimKey);

	if (nullptr != pAnim)
		return false;

	pAnim = new CAnimation2D;

	pAnim->Create(_pTex, _vLT, _vGap, _iFrmCount, _fDuration);

	m_mapAnim.insert(make_pair(_strAnimKey, pAnim));
	pAnim->SetAnimator2D(this);
	pAnim->SetName(_strAnimKey);

	return true;
}

void CAnimator2D::PlayAnimation(const wstring & _strAnimKey, bool _bRepeat)
{
	m_pCurAnim = FindAnimation(_strAnimKey);

	if (nullptr == m_pCurAnim)
	{
		//DBG_WARNNING(L"Can't Find Animation");
		return;
	}

	m_bRepeat = _bRepeat;
	m_bPlay = true;	
}

void CAnimator2D::StopAnimation()
{
	m_bPlay = false;
	if (nullptr == m_pCurAnim)
		return;
	
	m_pCurAnim->Play();	
}

CAnimation2D * CAnimator2D::FindAnimation(const wstring & _strKey)
{
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter == m_mapAnim.end())
		return nullptr;	

	return iter->second;
}

void CAnimator2D::UpdateData()
{
	if (m_pCurAnim) 
	{
		m_pCurAnim->UpdateData();
	}		
}

//void CAnimator2D::ClearData()
//{
//	//static CConstantBuffer* pAnimBuffer = CDevice::GetInst()->FindConstBuffer(L"Anim2D");
//	//
//	//static tAnim2D data = {};
//	//pAnimBuffer->SetData(&data);
//	//pAnimBuffer->UpdateData((UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);
//}

void CAnimator2D::SaveToScene(FILE * _pFile)
{
	UINT iType = (UINT)GetComponentType();
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_bRepeat, 1, 1, _pFile);

	size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, 4, 1, _pFile);

	for (auto& pair : m_mapAnim)
	{
		SaveWString(_pFile, pair.first);
		pair.second->SaveToScene(_pFile);
	}

	fwrite(&m_pCurAnim, sizeof(void*), 1, _pFile);
	if (nullptr != m_pCurAnim)
	{
		SaveWString(_pFile, m_pCurAnim->GetName());
	}	
}

void CAnimator2D::LoadFromScene(FILE * _pFile)
{
	fread(&m_bRepeat, 1, 1, _pFile);

	UINT iAnimCount = 0;
	fread(&iAnimCount, 4, 1, _pFile);

	for (UINT i = 0; i < iAnimCount; ++i)
	{
		wstring strAnimName = LoadWString(_pFile);
		CAnimation2D* pAnim = new CAnimation2D;
		pAnim->SetName(strAnimName);
		pAnim->LoadFromScene(_pFile);
		m_mapAnim.insert(make_pair(strAnimName, pAnim));
	}

	void* pData = nullptr;
	fread(&pData, sizeof(void*), 1, _pFile);

	if (pData)
	{
		wstring strCurAnim = LoadWString(_pFile);		
		m_pCurAnim = FindAnimation(strCurAnim);
		assert(m_pCurAnim);
	}
}