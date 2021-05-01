#include "pch.h"
#include "GameObj.h"

void GameObj::Draw(HDC hdc, HDC memDC)
{
	BitBlt(hdc, OFFSETX+posX, OFFSETY+posY, 16, 16, memDC, 0, 0, SRCCOPY);
}
