#pragma once

#define SINGLE(type) private: type(); ~type();\
public:\
static type* GetInst()\
{\
	static type mgr;\
	return &mgr;\
}

#define SAFE_RELEASE(p) if(nullptr != p) p->Release()
#define SAFE_DELETE(p) if(nullptr != p) delete p; p = nullptr;

#define DEVICE CDevice::GetInst()->GetDevice()
#define CMDLIST CDevice::GetInst()->GetCmdList()
#define CMDLIST_RES CDevice::GetInst()->GetCmdListRes()

#define KEY(Key, State) (CKeyMgr::GetInst()->GetKeyState(Key) == State)
#define KEY_HOLD(Key) KEY(Key, KEY_STATE::STATE_HOLD)
#define KEY_TAB(Key) KEY(Key, KEY_STATE::STATE_TAB)
#define KEY_AWAY(Key) KEY(Key, KEY_STATE::STATE_AWAY)
#define KEY_NONE(Key) KEY(Key, KEY_STATE::STATE_NONE)

#define DT CTimeMgr::GetInst()->GetDeltaTime()


typedef XMFLOAT4 Vec4;
typedef XMFLOAT3 Vec3;
typedef XMFLOAT2 Vec2;
typedef XMMATRIX Matrix;

enum class CONST_REGISTER
{
	b0 = 0,
	b1 = 1,
	b2 = 2,
	b3 = 3,
	b4 = 4,
	b5 = 5,

	END,
};

enum class TEXTURE_REGISTER
{
	t0 = (UINT)CONST_REGISTER::END,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,
	t11,
	t12,
	END,
};
enum class RES_TYPE
{
	MATERIAL,
	MESH,
	TEXTURE,
	SOUND,
	SHADER,
	END,
};

enum class ROOT_SIG_TYPE
{
	INPUT_ASSEM,
	CONST_BUFFER,
	END,
};

enum class RS_TYPE
{
	DEFAULT,
	END,
};

enum class BLEND_TYPE
{
	DEFAULT,
	END,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESHRENDER,



	END,
};


// State Description
extern D3D12_RASTERIZER_DESC g_arrRSDesc[(UINT)RS_TYPE::END];
extern D3D12_BLEND_DESC g_arrBlendDesc[(UINT)BLEND_TYPE::END];
