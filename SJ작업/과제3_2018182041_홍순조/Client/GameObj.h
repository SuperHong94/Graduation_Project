#pragma once
class GameObj
{
public:
	GameObj() {};
	void init()
	{
		posX = 0, posY = 0;
	}
	void GetPos(int& x, int& y) { x = posX; y = posY; };
	void SetPos(int x, int y) { posX = x; posY = y; };
	void Draw(HDC hdc, HDC memDC);
	int GetId() { return m_id; };
	void SetId(int id) { m_id = id; };
	void SetMe() { isMe = true; };
	bool isVisible = false;
	bool isMe = false;
private:
	int m_id;
	int posX = 0, posY = 0;

	

};

