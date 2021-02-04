#include "value.fx"

#ifndef _STD
#define _STD

struct VS_INPUT
{
    float3 vPos : POSITION; // sementic (������) ���� Layout �� ����       
    float4 vColor : COLOR;    
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 VBinormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 vOutPos : SV_Position; 
    float4 vOutColor : COLOR;
    float2 vUV : TEXCOORD;
};

// ==================
// Test Vertex Shader
// ==================
VS_OUTPUT VS_Test(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;  
    
    // ����(��) ��ǥ�� ��ȯ.
    // ���� w ��Ҹ� 1�� Ȯ���Ѵ� (���� ����� 4��(�̵�) �� ���� �޴´�.)
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    
    output.vOutPos = vWorldPos;
    output.vOutColor = _input.vColor;
    
    output.vUV = _input.vUV;

    return output;
}

// Rasterizer 
// �������̴����� ��ȯ�� ������ǥ�� ���ؼ�
// Target ���ۿ��� ȣ�� �Ǿ�� �ϴ� Pixel �� ã�Ƴ���.

// Cull Mode �ɼǿ� ����, ���� �� Toplogy �� �����Ѵ�.
// CULL_BACK(ccw)

// Pixel Shader
// Rasterizer ���� ������ �ȼ��鸶�� ȣ�� �Ǵ� �Լ�
// �������� ��ȯ�� ������ Ÿ�ٿ� ����Ѵ�.
float4 PS_Test(VS_OUTPUT _input) : SV_Target
{    
    if(g_int_0 == 1)
        return float4(1.f, 0.2f, 0.2f, 1.f);
    if (g_int_0 == 2)
        return float4(0.2f, 0.2f, 1.f, 1.f);
    else   
        return g_tex_0.Sample(g_sam_0, _input.vUV);

   // return g_tex_0.Sample(g_sam_0, _input.vUV);
}

#endif