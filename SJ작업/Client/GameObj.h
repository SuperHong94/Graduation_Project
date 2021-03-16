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
private:
	int posX = 0, posY = 0;

};

