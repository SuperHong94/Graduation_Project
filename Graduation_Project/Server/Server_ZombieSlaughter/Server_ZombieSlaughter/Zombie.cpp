#include "stdafx.h"
#include "Zombie.h"

CZombie::CZombie()
{


}



CZombie::~CZombie()
{
}

void CZombie::init()
{
	//랜덤으로 만들자.
	float randomXPos = rand() % 9000 - 4500;
	float randomZPos = rand() % 9000 - 4500;
	SetPostion(Vec3(randomXPos, 0.f, randomZPos));
	m_eMosterState = MonsterState::M_Respawn;

}
