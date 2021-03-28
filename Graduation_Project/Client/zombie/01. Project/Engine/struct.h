#pragma once

#include "define.h"

struct tResolution
{
	float fWidth;
	float fHeight;
};

// 정점 구조체
struct VTX
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;

	Vec3 vTangent;
	Vec3 vNormal;	
	Vec3 vBinormal;

	Vec4 vWeights;
	Vec4 vIndices;
};

struct tDBG
{
	DBG_TYPE eType;
	wstring  strDBG;
};


struct tLightColor
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
};

struct tParticle
{
	Vec3 vWorldPos;		// 위치	
	Vec3 vWorldDir;		// 이동 방향

	float m_fCurTime;	// 현재 시간
	float m_fLifeTime;	// 최대 생명주기

	int  iAlive;
	int  arrPading[3];
};

struct tParticleShared
{
	int iAddCount;
	int arrPading[3];
};


// 상수버퍼 메모리
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
	Matrix matWV;
	Matrix matWVP;
	Matrix matWorldInv;
	Matrix matViewInv;
	Matrix matProjInv;
};

extern tTransform g_transform;

//struct tTransform
//{
//	Matrix matWorld;
//	Matrix matView;
//	Matrix matProj;
//
//	Matrix matWV;
//	Matrix matWVP;
//};


struct tMtrlParam
{
	Vec4			m_vDiff; // 색상 계수
	Vec4			m_vSpec; // 반사광 계수
	Vec4			m_vEmv;  // 발광 계수

	int				m_arrInt[(UINT)SHADER_PARAM::INT_END - (UINT)SHADER_PARAM::INT_0];
	float			m_arrFloat[(UINT)SHADER_PARAM::FLOAT_END - (UINT)SHADER_PARAM::FLOAT_0];
	Vec2			m_arrVec2[(UINT)SHADER_PARAM::VEC2_END - (UINT)SHADER_PARAM::VEC2_0];
	Vec4			m_arrVec4[(UINT)SHADER_PARAM::VEC4_END - (UINT)SHADER_PARAM::VEC4_0];
	Matrix			m_arrMat[(UINT)SHADER_PARAM::MATRIX_END - (UINT)SHADER_PARAM::MATRIX_0];

	int				m_iArrTex[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0];
};


struct tAnim2D
{
	Vec2 vLT;			// UV 좌상단
	Vec2 vLen;			// UV 길이
	Vec2 vOffset;		// Offset Pos
	int  IsAnim2D;		// Animation2D 컴포넌트 사용여부
	int  iPadding;
};

struct tLight2D
{
	Vec4 vLightPos;
	Vec4 vLightColor;
	Vec4 vLightDir;
	int   iLightType;
	float fRange;
	float fAngle;
	int   ipadding;
};

struct tLight2DInfo
{
	tLight2D	arrLight2D[100];
	UINT		iCount;
};

struct tLight3D
{
	tLightColor tColor;
	Vec4		vLightPos;
	Vec4		vLightDir;
	int			iLightType;
	float		fRange;
	float		fAngle;
	int			iPadding;
};

struct tLight3DInfo
{
	tLight3D arrLight3D[100];
	UINT	 iCurCount;
	Vec3     vPadding;
};


struct tGlobalValue
{
	tResolution vResolution;
	float fDT;
	float fAccTime;
};


//================
// Struct of FBX 
//=================
typedef struct _tagMtrlInfo
{
	Vec4	vMtrlDiff;
	Vec4	vMtrlSpec;
	Vec4	vMtrlAmb;
	Vec4	vMtrlEmiv;
	_tagMtrlInfo()
		: vMtrlDiff(1.f, 1.f, 1.f, 1.f)
		, vMtrlSpec(1.f, 1.f, 1.f, 1.f)
		, vMtrlAmb(1.f, 1.f, 1.f, 1.f)
		, vMtrlEmiv(1.f, 1.f, 1.f, 1.f)
	{}

}tMtrlInfo;

typedef struct _tagFbxMat
{
	tMtrlInfo	tMtrl;
	wstring     strMtrlName;
	wstring     strDiff;
	wstring		strNormal;
	wstring		strSpec;
}tFbxMaterial;

typedef struct _tagWeightsAndIndices
{
	int		iBoneIdx;
	double	dWeight;
}tWeightsAndIndices;

typedef struct _tagContainer
{
	wstring								strName;
	vector<Vec3>						vecPos;
	vector<Vec3>						vecTangent;
	vector<Vec3>						vecBinormal;
	vector<Vec3>						vecNormal;
	vector<Vec2>						vecUV;

	vector<Vec4>						vecIndices;
	vector<Vec4>						vecWeights;

	vector<vector<UINT>>				vecIdx;
	vector<tFbxMaterial>				vecMtrl;

	// Animation 관련 정보
	bool								bAnimation;
	vector<vector<tWeightsAndIndices>>	vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}

}tContainer;

// Animation
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};

struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Offset 행렬(뼈 -> 루트 까지의 행렬)
	Matrix				matBone;   // 이거 안씀
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring		strAnimName;
	int			iStartFrame;
	int			iEndFrame;
	int			iFrameLength;

	double		dStartTime;
	double		dEndTime;
	double		dTimeLength;
	float		fUpdateTime; // 이거 안씀

	FbxTime::EMode eMode;
};