#include "value.fx"

#ifndef _STD
#define _STD

struct VS_INPUT
{
    float3 vPos : POSITION; // sementic (지시자) 정점 Layout 과 연동       
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
    
    // 로컬(모델) 좌표의 변환.
    // 따라서 w 요소를 1로 확장한다 (월드 행렬의 4행(이동) 을 적용 받는다.)
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    
    output.vOutPos = vWorldPos;
    output.vOutColor = _input.vColor;
    
    output.vUV = _input.vUV;

    return output;
}

// Rasterizer 
// 정점쉐이더에서 반환한 투영좌표를 통해서
// Target 버퍼에서 호출 되어야 하는 Pixel 을 찾아낸다.

// Cull Mode 옵션에 따라, 제거 할 Toplogy 를 선별한다.
// CULL_BACK(ccw)

// Pixel Shader
// Rasterizer 에서 검출한 픽셀들마다 호출 되는 함수
// 정점에서 반환한 색상값을 타겟에 출력한다.
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