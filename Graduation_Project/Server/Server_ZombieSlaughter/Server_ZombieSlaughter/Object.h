#pragma once
class CObject
{
protected:
	Vec3 m_postion;
	Vec3 m_rotation;
	

public:
	CObject();
	virtual ~CObject();
	virtual void init()=0; 
	virtual void Update() = 0;
	Vec3 GetPostion() { return m_postion; };
	void SetPostion(Vec3 pos) { m_postion = pos; };
	Vec3 GetRotation() { return m_rotation; };
	void SetRotation(Vec3 rot) { m_rotation = rot; };



};

