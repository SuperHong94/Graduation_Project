#pragma once

struct tResolution
{
	float fWidth;
	float fHeight;
};

// ���� ����ü
struct VTX
{
	Vec3 vPos;
	Vec4 vColor;
};



// ������� �޸�
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};