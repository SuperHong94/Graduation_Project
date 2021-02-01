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
};



// 상수버퍼 메모리
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};