#pragma once
#include "Script.h"
#include "Scene.h"


struct PlayerStatus
{
	PlayerState state = P_Spawn;
	BulletState bulletState = B_Normal;
	float hp = 100.f;
	//float speed = 4000.f;
	float speed = 300.f;
};


class CPlayerScript :
	public CScript
{
private:
	Ptr<CMaterial>		m_pOriginMtrl;
	Ptr<CMaterial>		m_pCloneMtrl;

	float bulletHeight = 0;

	CGameObject* pObject;
	PlayerStatus status;

	PlayerState previousState = P_Spawn; // 플레이어의 이전 프레임 애니메이션 상태


	bool isPlayer;
public:
	virtual void awake();
	virtual void update();

	PlayerState setRunAni(Vec3 dir, Vec3 axis);
public:
	CLONE(CPlayerScript);

public:
	CPlayerScript(CGameObject* Object, bool player);
	virtual ~CPlayerScript();
};

