#pragma once
class CBullet: protected CObject
{
private:
	float damage;
	BulletState eBulletState;
	bool BulletActive = false;
	float AddDamage = 0.f;
	int bullet_ID; //배열로 bullet 선언했을때 1씩 증가 시킬려고 함
	int speed; //총알속도

public:
	CBullet();
	virtual ~CBullet();
	virtual void init();
	virtual void Update();
};

