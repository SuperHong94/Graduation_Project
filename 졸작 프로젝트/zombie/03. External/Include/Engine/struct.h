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
	Vec2 vUV;
};

struct CONST_BUFFER_INFO
{
	UINT				iRegisterNum;// �������� ��ȣ
	UINT				iCount;		 // ���� ����
	UINT				iBufferSize; // ���� ���� ������	
};


// ������� �޸�
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