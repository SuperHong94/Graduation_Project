#include "stdafx.h"
#include "PlayerObject.h"

void CPlayerObject::init()
{
	m_postion = { 0.f,0.f,0.f };
	m_speed = 1000;
}

void CPlayerObject::Update(c2s_Key* packet)
{

	EKEY_EVENT send_key;
	float dt = packet->dT;
	switch (packet->key)
	{
	case DOWN_UP:
		m_postion.z += dt * m_speed;
		send_key = EKEY_EVENT::DOWN_UP;
		break;
	case DOWN_DOWN:
		m_postion.z -= dt * m_speed;
		send_key = EKEY_EVENT::DOWN_DOWN;
		break;
	case DOWN_RIGHT:
		m_postion.x += dt * m_speed;
		send_key = EKEY_EVENT::DOWN_RIGHT;
		break;
	case DOWN_LEFT:
		m_postion.x -= dt * m_speed;
		send_key = EKEY_EVENT::DOWN_LEFT;
		break;
	default:
		break;
	}
	if (m_postion.x > 4990)
		m_postion.x = 4990;
	if (m_postion.x < -4990)
		m_postion.x = -4990;
	if (m_postion.z > 4990)
		m_postion.z = 4990;
	if (m_postion.z < -4990)
		m_postion.z = -4990;
}
