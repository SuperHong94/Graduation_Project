#pragma once
class CBullet: protected CObject
{
private:
	float damage;
	BulletState eBulletState;
	bool BulletActive = false;
	float AddDamage = 0.f;
	int bullet_ID; //�迭�� bullet ���������� 1�� ���� ��ų���� ��
	int speed; //�Ѿ˼ӵ�

public:
	CBullet();
	virtual ~CBullet();
	virtual void init();
	virtual void Update();
};

