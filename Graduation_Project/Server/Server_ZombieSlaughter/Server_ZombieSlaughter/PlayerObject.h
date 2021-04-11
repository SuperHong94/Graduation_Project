#pragma once
class CPlayerObject
{
private:
	Vec3 m_postion;
	int m_speed;

public:
	void init();
	Vec3 GetPostion() { return m_postion; };
	void SetPostion(Vec3 pos) { m_postion = pos; };

	int GetSpeed() { return m_speed; };
	void SetSpped(int speed) { m_speed = speed; };
};

