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


#define CLONE(type) public: type* Clone() { return new type(*this); }
#define CLONE_DISABLE(type) type* Clone() { assert(nullptr); return nullptr;} \
                            type(const type& _class){assert(nullptr);}

#define DBG_WARNNING(pStr) CDebugMgr::GetInst()->AddDbgMsg(DBG_TYPE::DBG_WARNNING, pStr);
#define DBG_ERROR(pStr) CDebugMgr::GetInst()->AddDbgMsg(DBG_TYPE::DBG_ERROR, pStr);
#define DBG_MSG(pStr) CDebugMgr::GetInst()->AddDbgMsg(DBG_TYPE::DBG_MSG, pStr);

#define MAX_LAYER 32

typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;
using DirectX::SimpleMath::Matrix;

enum class CONST_REGISTER
{
	b0 = 0,
	b1 = 1,
	b2 = 2,
	b3 = 3,
	b4 = 4,	
	END,	
};

enum class TEXTURE_REGISTER
{
	t0	= (UINT)CONST_REGISTER::END,
	t1	, 
	t2	, 
	t3	, 
	t4	, 
	t5	, 
	t6	, 
	t7	, 
	t8	, 
	t9	, 
	t10 , 
	t11 , 
	t12 , 
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
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,
	END,
};


namespace RES_TYPE_STR
{
	extern const wchar_t* MATERIAL;
	extern const wchar_t* MESH;
	extern const wchar_t* TEXTURE;
	extern const wchar_t* SOUND;
	extern const wchar_t* SHADER;
}

extern const wchar_t* RES_TYPE_NAME[(UINT)RES_TYPE::END];


enum class SHADER_TYPE
{
	ST_VERTEX = 0x01,
	ST_HULL = 0x02,
	ST_DOMAIN = 0x04,
	ST_GEOMETRY = 0x08,
	ST_COMPUTE = 0x10,
	ST_PIXEL = 0x20,
	ST_END
};

#define ST_ALL (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_HULL\
 | (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_GEOMETRY | (UINT)SHADER_TYPE::ST_COMPUTE | (UINT)SHADER_TYPE::ST_PIXEL

enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESHRENDER,
	CAMERA,
	COLLIDER2D,
	COLLIDER3D,
	ANIMATOR2D,
	ANIMATOR3D,
	LIGHT2D,
	LIGHT3D,
	END,
	SCRIPT,
};

enum class COLLIDER2D_TYPE
{
	RECT,
	CIRCLE,
};

enum class SHADER_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,
	INT_END,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,
	FLOAT_END,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,
	VEC2_END,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,
	VEC4_END,

	MATRIX_0,
	MATRIX_1,
	MATRIX_2,
	MATRIX_3,
	MATRIX_END,

	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_END,
};


enum class BLEND_TYPE
{
	DEFAULT,
	ALPHABLEND,
	ONEBLEND,
	END,
};

enum class DEPTH_STENCIL_TYPE
{
	LESS,
	LESS_EQUAL,
	GRATER,
	GRATER_EQUAL,

	NO_DEPTHTEST,
	NO_DEPTHTEST_NO_WRITE,

	END,
};

enum class DBG_TYPE
{
	DBG_ERROR,
	DBG_WARNNING,
	DBG_MSG,
};

enum class EVENT_TYPE
{
	CREATE_OBJECT,	// wParam : GameObject, lParam : Layer Idx
	DELETE_OBJECT,	// wParam : GameObject,
	ADD_CHILD,		// wParam : Parent Object, lParam : Child Object
	CLEAR_PARENT,	// wParam : Target Object
	TRANSFER_LAYER,	// wParam : Target Object, lParam : (HIWORD)Layer Index (LOWORD)bMoveAll
	TRANSFER_SCENE,
	ACTIVATE_GAMEOBJECT,	// wParam : GameObject Adress
	DEACTIVATE_GAMEOBJECT,	// wParam : GameObject Adress

	ACTIVATE_COMPONENT,		// wParam : Component Adress
	DEACTIVATE_COMPONENT,	// wParam : Component Adress

	END,
};

enum class LIGHT_TYPE
{
	DIR,
	POINT,
	SPOT,
	END,
};

enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,
	END,
};

enum class FACE_TYPE
{
	FT_NEAR,
	FT_FAR,
	FT_UP,
	FT_DOWN,
	FT_LEFT,
	FT_RIGHT,
	FT_END,
};

enum class RT_TYPE
{
	SWAPCHAIN,

	DIFFUSE,
	NORMAL,
	POSITION,

	LIGHT,
	SPECULAR,

	SHADOWMAP,

	POSTEFFECT1,
	POSTEFFECT2,

	END,
};

enum class MRT_TYPE
{
	// MRT			RT а╤гу
	SWAPCHAIN,  // SWAPCHAIN
	DEFERRED,	// DIFFUSE, NORMAL, POSITION
	LIGHT,		// LIGHT, SPECULAR
	SHADOWMAP,	// SHADOWMAP
	POSTEFFECT, // POSTEFFECT1, POSTEFFECT2
	END,
};


