#pragma once
class CPlayerObject
{
private:
	Vec3 m_postion;
	Vec3 m_rotation;
	int m_speed;

public:
	void init();
	Vec3 GetPostion() { return m_postion; };
	void SetPostion(Vec3 pos) { m_postion = pos; };
	void Update(c2s_Key* packet);
	Vec3 GetRotation() { return m_rotation; };
	void SetRotation(Vec3 rot) { m_rotation = rot; };

	int GetSpeed() { return m_speed; };
	void SetSpped(int speed) { m_speed = speed; };
};

