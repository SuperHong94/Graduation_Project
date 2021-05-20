#include "stdafx.h"
#include "Entity.h"

UINT CEntity::g_iID = 0;

CEntity::CEntity()
	: m_iID(g_iID++)
{
}


CEntity::CEntity(const CEntity & _other)
	: m_strName(_other.m_strName)
	, m_iID(g_iID++)
{
}

CEntity::~CEntity()
{
}
