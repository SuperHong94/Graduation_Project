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
	//�������� ������.
	float randomXPos = rand() % 9000 - 4500;
	float randomZPos = rand() % 9000 - 4500;
	SetPostion(Vec3(randomXPos, 0.f, randomZPos));
	m_eMosterState = MonsterState::M_Respawn;

}
