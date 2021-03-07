#pragma once

union tColID
{
	struct {
		UINT iColID1;
		UINT iColID2;
	};
	DWORD_PTR ID;
};

class CCollider2D;
class CLayer;

class CCollisionMgr
{
	SINGLE(CCollisionMgr);
private:
	UINT					m_LayerCheck[MAX_LAYER];
	map<DWORD_PTR, bool>	m_mapCol;

public:
	void init();
	void update();
	
public:
	void CheckCollisionLayer(const wstring& _strLayerName1, const wstring& _strLayerName2);
	void CheckCollisionLayer(int _iLayerIdx1, int _iLyaerIdx2);

private:
	void CollisionLayer(const CLayer* _pLayer1, const CLayer* _pLayer2);
	bool IsCollision(CCollider2D* _pCollider1, CCollider2D* _pCollider2);
	bool CollisionRect(CCollider2D* _pCollider1, CCollider2D* _pCollider2);
	bool CollisionCircle(CCollider2D* _pCollider1, CCollider2D* _pCollider2);
	bool CollisionRectCircle(CCollider2D* _pCollider1, CCollider2D* _pCollider2);
};

