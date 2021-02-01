#include "stdafx.h"
#include "Resource.h"


CResource::CResource(RES_TYPE _eType)
	: m_iRefCount(0)
	, m_eType(_eType)
{
}

CResource::~CResource()
{
}
