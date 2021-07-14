#include "stdafx.h"
#include "Zombie.h"

CZombie::CZombie()
{


}



CZombie::~CZombie()
{
}

void CZombie::init(int id)
{
	//랜덤으로 만들자.
	float randomXPos = rand() % 9000 - 4500;
	float randomZPos = rand() % 9000 - 4500;
	SetPostion(Vec3(randomXPos, 0.f, randomZPos));
	m_eMosterState = MonsterState::M_Respawn;
	isActive = true;
	m_id = id;
	m_last_move_time = chrono::high_resolution_clock::now(); //현재시각

	//임시 방향 설정 //원점으로 향하게 설정
	targetDir = { 0.f-randomXPos,0.0f,0.0-randomZPos };
	targetDir.Normalize();
}

chrono::high_resolution_clock::time_point CZombie::getLastTime()
{
	return m_last_move_time;
}

void CZombie::move2target()
{
	m_postion = m_postion + (targetDir * speed);
}

void CZombie::setLastTime(high_resolution_clock::time_point tp)
{
	m_last_move_time = tp;
}
