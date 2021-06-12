#ifndef _POSTEFFECT
#define _POSTEFFECT

#include "value.fx"
#include "func.fx"


struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vOutPos : SV_Position;
    float2 vOutUV : TEXCOORD;
};

//========================
// Distortion Shader
// AlphaBlend = false
// PostEffect = true
// g_Tex_0 : Posteffect Target
//========================

VS_OUTPUT VS_Distortion(VS_INPUT _input)
{
    // input 으로 들어온 위치정보를 투영좌표계 까지 변환한다.
    VS_OUTPUT output = (VS_OUTPUT)0.f;

    output.vOutPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vOutUV = _input.vUV;

    return output;
}

struct PS_STD3D_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
};

float4 PS_Distortion(VS_OUTPUT _input) : SV_Target
{
    float2 vScreenUV = float2(_input.vOutPos.x / g_vResolution.x, _input.vOutPos.y / g_vResolution.y);
    //return g_tex_0.Sample(g_sam_0, vScreenUV);


    float2 vDir = normalize(float2(0.5f, 0.5f) - _input.vOutUV);

    float fDist = distance(float2(0.5f, 0.5f), _input.vOutUV);

    // 왜곡 강도에 영향을 주는 중심으로부터 떨어진 비율( 중심에 가까울 수록 0에 가깝다.)
    float fRatio = (fDist / 0.5f);
    fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

    vScreenUV += vDir * sin(abs((fRatio - g_fAccTime * 0.06f) * 80.f)) * 0.03f;

    PS_STD3D_OUT output = (PS_STD3D_OUT)0.f;
    output.vDiffuse = g_tex_0.Sample(g_sam_0, vScreenUV);

    return g_tex_0.Sample(g_sam_0, vScreenUV);
}

//========================
// Distortion Character
// AlphaBlend = false
// PostEffect = true
// g_Tex_0 : Posteffect Texture
//========================
struct VS_DS_CHAR_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    float4 vWeight : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_DS_CHAR_OUTPUT
{
    float4 vOutPos : SV_Position;
    float2 vOutUV : TEXCOORD;
};

VS_DS_CHAR_OUTPUT VS_DistortionCharacter(VS_DS_CHAR_INPUT _input)
{
    // input 으로 들어온 위치정보를 투영좌표계 까지 변환한다.
    VS_DS_CHAR_OUTPUT output = (VS_DS_CHAR_OUTPUT)0.f;

    if (g_int_0)
    {
        Skinning(_input.vPos, _input.vWeight, _input.vIndices, 0);
    }

    output.vOutPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vOutUV = _input.vUV;

    return output;
}

float4 PS_DistortionCharacter(VS_DS_CHAR_OUTPUT _input) : SV_Target
{
    float2 vScreenUV = float2(_input.vOutPos.x / g_vResolution.x, _input.vOutPos.y / g_vResolution.y);

    float2 vDir = normalize(float2(0.5f, 0.5f) - vScreenUV);
    float fDist = distance(float2(0.5f, 0.5f), vScreenUV);

    // 왜곡 강도에 영향을 주는 중심으로부터 떨어진 비율( 중심에 가까울 수록 0에 가깝다.)
    float fRatio = (fDist / 0.5f);
    fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

    //                                                 속도     진폭                                         
    vScreenUV += vDir * sin(abs((fRatio - g_fAccTime * 0.06f) * 40.f)) * 0.03f;

    PS_STD3D_OUT output = (PS_STD3D_OUT)0.f;
    output.vDiffuse = g_tex_0.Sample(g_sam_0, vScreenUV);

    return g_tex_0.Sample(g_sam_0, vScreenUV);
}

#endif