#include "stdafx.h"
#include "PlayerObject.h"

void CPlayerObject::init() //게임들어갈때 초기화
{

	m_postion = { 2000.f,0.f,2000.f };

	m_rotation = { 0.f,0.f,0.f };
	m_speed = 500;
	m_eSceneState = SCENE_STATE::START_SCENE; 
}

void CPlayerObject::Update(c2s_Key* packet)
{

	EKEY_EVENT send_key;
	float dt = packet->dT;
	switch (packet->key)
	{
	case DOWN_UP:
		m_postion.z += dt * m_speed;

		m_ePState = PlayerState::P_FRun;
		break;
	case DOWN_DOWN:
		m_postion.z -= dt * m_speed;

		m_ePState = PlayerState::P_BRun;
		break;
	case DOWN_RIGHT:
		m_postion.x += dt * m_speed;

		m_ePState = PlayerState::P_RRun;
		break;
	case DOWN_LEFT:
		m_postion.x -= dt * m_speed;

		m_ePState = PlayerState::P_LRun;
		break;
	case NO_EVENT:

		m_rotation = { packet->rX,packet->rY,packet->rZ };
		m_ePState = PlayerState::P_Idle;
		break;
	default:
		break;
	}

	if (m_postion.x > MAX_MAP)
		m_postion.x = MAX_MAP;
	if (m_postion.x < MIN_MAP)
		m_postion.x = MIN_MAP;
	if (m_postion.z > MAX_MAP)
		m_postion.z = MAX_MAP;
	if (m_postion.z < MIN_MAP)
		m_postion.z = MIN_MAP;


}
