#pragma once

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
};

struct CONST_BUFFER_INFO
{
	UINT				iRegisterNum;// 레지스터 번호
	UINT				iCount;		 // 버퍼 개수
	UINT				iBufferSize; // 버퍼 단일 사이즈	
};


// 상수버퍼 메모리
struct tTransform
{
	Vec4 vOffset;
	/*Matrix matWorld;	
	Matrix matWorldInv;
	Matrix matView;
	Matrix matViewInv;
	Matrix matProj;
	Matrix matWV;
	Matrix matWVP;*/
};