#include "stdafx.h"
#include "CollisionMgr.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Collider2D.h"


CCollisionMgr::CCollisionMgr()
	: m_LayerCheck{}

{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::init()
{
}

void CCollisionMgr::update()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		for (int j = 0; j < MAX_LAYER; ++j)
		{			
			if (m_LayerCheck[i] & (1 << j))
			{
				// i <= j
				CollisionLayer(pCurScene->GetLayer(i), pCurScene->GetLayer(j));				
			}
		}
	}
}

void CCollisionMgr::CheckCollisionLayer(const wstring & _strLayerName1, const wstring & _strLayerName2)
{
	CLayer* pLayer1 = CSceneMgr::GetInst()->GetCurScene()->FindLayer(_strLayerName1);
	CLayer* pLayer2 = CSceneMgr::GetInst()->GetCurScene()->FindLayer(_strLayerName2);

	assert(nullptr != pLayer1 && nullptr != pLayer2);

	int iIdx1 = pLayer1->GetLayerIdx();
	int iIdx2 = pLayer2->GetLayerIdx();

	int iMinIdx = min(iIdx1, iIdx2);
	int iMaxIdx = max(iIdx1, iIdx2);

	m_LayerCheck[iMinIdx] |= (1 << iMaxIdx);
}

void CCollisionMgr::CheckCollisionLayer(int _iLayerIdx1, int _iLyaerIdx2)
{
	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();

	assert(pScene->GetLayer(_iLayerIdx1) && pScene->GetLayer(_iLyaerIdx2));

	int iMinIdx = min(_iLayerIdx1, _iLyaerIdx2);
	int iMaxIdx = max(_iLayerIdx1, _iLyaerIdx2);

	m_LayerCheck[iMinIdx] |= (1 << iMaxIdx);
}

void CCollisionMgr::CollisionLayer(const CLayer * _pLayer1, const CLayer * _pLayer2)
{
	const vector<CGameObject*>& vecObj1 = _pLayer1->GetObjects();
	const vector<CGameObject*>& vecObj2 = _pLayer2->GetObjects();

	map<DWORD_PTR, bool>::iterator iter;

	for (size_t i = 0; i < vecObj1.size(); ++i)
	{
		CCollider2D* pCollider1 = vecObj1[i]->Collider2D();

		if (nullptr == pCollider1)
			continue;

		size_t j = 0;		
		if (_pLayer1 == _pLayer2) // 동일한 레이어 간의 충돌을 검사하는 경우
			j = i + 1;

		for (; j < vecObj2.size(); ++j)
		{			
			CCollider2D* pCollider2 = vecObj2[j]->Collider2D();

			if (nullptr == pCollider2)
				continue;

			tColID id;
			id.iColID1 = pCollider1->GetColID();
			id.iColID2 = pCollider2->GetColID();
			iter = m_mapCol.find(id.ID);

			bool IsDead = false;
			if (pCollider1->GetObj()->IsDead() || pCollider2->GetObj()->IsDead())
				IsDead = true;

			// 충돌했다.
			if (IsCollision(pCollider1, pCollider2))
			{
				// 충돌 중이다
				if (m_mapCol.end() != iter && iter->second == true)
				{
					if (IsDead)
					{
						pCollider1->OnCollisionExit(pCollider2);
						pCollider2->OnCollisionExit(pCollider1);
						iter->second = false;
					}
					else
					{
						pCollider1->OnCollision(pCollider2);
						pCollider2->OnCollision(pCollider1);
					}					
				}
				// 처음 충돌했다
				else
				{
					if (IsDead)
						continue;

					pCollider1->OnCollisionEnter(pCollider2);
					pCollider2->OnCollisionEnter(pCollider1);

					if (m_mapCol.end() == iter)
					{
						m_mapCol.insert(make_pair(id.ID, true));
					}
					else
					{
						iter->second = true;
					}
				}
			}
			else // 충돌하지 않는다.
			{
				if (m_mapCol.end() != iter && true == iter->second)
				{
					pCollider1->OnCollisionExit(pCollider2);
					pCollider2->OnCollisionExit(pCollider1);

					iter->second = false;
				}
			}
		}
	}
}

bool CCollisionMgr::IsCollision(CCollider2D * _pCollider1, CCollider2D * _pCollider2)
{
	if (!_pCollider1->IsActive() || !_pCollider1->GetObj()->IsActive() || !_pCollider2->IsActive() || !_pCollider2->GetObj()->IsActive())
		return false;

	if (COLLIDER2D_TYPE::RECT == _pCollider1->GetColliderType() && COLLIDER2D_TYPE::RECT == _pCollider2->GetColliderType())
	{
		return CollisionRect(_pCollider1, _pCollider2);
	}
	else if (COLLIDER2D_TYPE::CIRCLE == _pCollider1->GetColliderType() && COLLIDER2D_TYPE::CIRCLE == _pCollider2->GetColliderType())
	{
		return CollisionCircle(_pCollider1, _pCollider2);
	}
	else
	{
		return CollisionRectCircle(_pCollider1, _pCollider2);
	}	

	return false;
}

bool CCollisionMgr::CollisionRect(CCollider2D * _pCollider1, CCollider2D * _pCollider2)
{
	static Vec3 arrLocal[4] = {					// 0 -- 1
		  Vec3(-0.5f, 0.5f, 0.f)				// |	|
		, Vec3(0.5f, 0.5f, 0.f)					// 3 -- 2
		, Vec3(0.5f, -0.5f, 0.f)
		, Vec3(-0.5f, -0.5f, 0.f)};	
	

	const Matrix& matCol1 = _pCollider1->GetColliderWorldMat();
	const Matrix& matCol2 = _pCollider2->GetColliderWorldMat();

	Vec3 arrCol1[4] = {};
	Vec3 arrCol2[4] = {};
	Vec3 arrCenter[2] = {};
	
	for (UINT i = 0; i < 4; ++i)
	{
		arrCol1[i] = XMVector3TransformCoord(arrLocal[i], matCol1);
		arrCol2[i] = XMVector3TransformCoord(arrLocal[i], matCol2);

		// 2D 충돌이기 때문에 같은 Z 좌표상에서 충돌을 계산한다.
		arrCol1[i].z = 0.f;
		arrCol2[i].z = 0.f;
	}

	arrCenter[0] = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol1);
	arrCenter[1] = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matCol2);
	arrCenter[0].z = 0.f;
	arrCenter[1].z = 0.f;

	Vec3 vCenter = arrCenter[1] - arrCenter[0];
	
	Vec3 arrOriginVec[4] = { arrCol1[3] - arrCol1[0]
		, arrCol1[1] - arrCol1[0]
		, arrCol2[3] - arrCol2[0]
		, arrCol2[1] - arrCol2[0]
	};

	Vec3 arrProjVec[4] = {};
	for (UINT i = 0; i < 4; ++i)
	{
		arrOriginVec[i].Normalize(arrProjVec[i]);
	}


	// 투영을 통해서 분리축 테스트
	// vCenter		 두 사각형의 중심을 잇는 벡터
	// arrOriginVec  각 사각형의 표면 벡터
	// arrProjVec    사각형의 표면과 평행한 투영축 벡터(단위벡터)

	for (UINT i = 0; i < 4; ++i)
	{
		float fCenter = abs(vCenter.Dot(arrProjVec[i])); // 중심 거리 벡터를 해당 투영축으로 투영시킨 길이
		
		float fAcc = 0.f;
		for (UINT j = 0; j < 4; ++j)
			fAcc += abs(arrOriginVec[j].Dot(arrProjVec[i]));

		fAcc /= 2.f;

		if (fCenter > fAcc)
			return false;
	}

	return true;
}

bool CCollisionMgr::CollisionCircle(CCollider2D * _pCollider1, CCollider2D * _pCollider2)
{
	return false;
}

bool CCollisionMgr::CollisionRectCircle(CCollider2D * _pCollider1, CCollider2D * _pCollider2)
{
	return false;
}
