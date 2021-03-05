#include "stdafx.h"
#include "ToolCamScript.h"

#include "Camera.h"

CToolCamScript::CToolCamScript(CGameObject* _pShootObject)
	: CScript(0)
	, m_fSpeed(200.f)
	, m_fScaleSpeed(1.f)
{
	m_pShootObject = _pShootObject;
}

CToolCamScript::~CToolCamScript()
{
}

void CToolCamScript::update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	float fScale = Camera()->GetScale();
	float fSpeed = m_fSpeed;

	//if (KEY_HOLD(KEY_TYPE::KEY_LSHIFT))
	//{
	//	fSpeed *= 5.f;
	//}

	//if (KEY_HOLD(KEY_TYPE::KEY_W))
	//{
	//	Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
	//	vPos += vFront * fSpeed * DT;
	//}

	//if (KEY_HOLD(KEY_TYPE::KEY_S))
	//{
	//	Vec3 vBack = -Transform()->GetWorldDir(DIR_TYPE::FRONT);
	//	vPos += vBack * fSpeed * DT;
	//}

	//if (KEY_HOLD(KEY_TYPE::KEY_A))
	//{
	//	Vec3 vLeft = -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	//	vPos += vLeft * fSpeed * DT;
	//}

	//if (KEY_HOLD(KEY_TYPE::KEY_D))
	//{
	//	Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	//	vPos += vRight * fSpeed * DT;
	//}

	if (KEY_HOLD(KEY_TYPE::KEY_1))
	{
		fScale -= m_fScaleSpeed * DT;
		Camera()->SetScale(fScale);
	}

	if (KEY_HOLD(KEY_TYPE::KEY_2))
	{
		fScale += m_fScaleSpeed * DT;
		Camera()->SetScale(fScale);
	}

	//if (KEY_HOLD(KEY_TYPE::KEY_RBTN))
	//{
	//	Vec2 vDrag = CKeyMgr::GetInst()->GetDragDir();
	//	Vec3 vRot = Transform()->GetLocalRot();

	//	vRot.x -= vDrag.y * DT * 3.f;
	//	vRot.y += vDrag.x * DT * 1.5f;

	//	Transform()->SetLocalRot(vRot);
	//}

	if (KEY_HOLD(KEY_TYPE::KEY_ADD) || KEY_HOLD(KEY_TYPE::KEY_PRIOR))
	{
		if (m_fZoomScaleY >= 0.3)
		{
			m_fZoomScaleY -= m_fZoomSpeed;
			m_fZoomScaleZ -= m_fZoomSpeed;
		}
	}

	if (KEY_HOLD(KEY_TYPE::KEY_SUBTRACT) || KEY_HOLD(KEY_TYPE::KEY_NEXT))
	{
		if (m_fZoomScaleY <= 1.3)
		{
			m_fZoomScaleY += m_fZoomSpeed;
			m_fZoomScaleZ += m_fZoomSpeed;
		}
	}

	Vec3 CameraDir;

	CameraDir.x = 0.785;
	CameraDir.y = 0;
	CameraDir.z = 0;

	Transform()->SetLocalRot(CameraDir);

	Vec3 vPlayerPos = m_pShootObject->Transform()->GetLocalPos();

	vPos.x = vPlayerPos.x;
	vPos.y = vPlayerPos.y + 500 * m_fZoomScaleY;
	vPos.z = vPlayerPos.z - 500 * m_fZoomScaleZ;

	Transform()->SetLocalPos(vPos);
}
