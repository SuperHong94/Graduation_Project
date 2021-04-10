#pragma once
class CPlayerObject
{
private:
	Vec3 m_postion;


public:
	Vec3 GetPostion() { return m_postion; };
	void SetPostion(Vec3 pos) { m_postion = pos; };
};

