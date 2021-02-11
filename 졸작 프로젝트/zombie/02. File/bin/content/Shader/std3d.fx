#include "value.fx"
#include "func.fx"

#ifndef _STD3D
#define _STD3D

// ==========================
// Std3D Shader
// 
// g_tex_0 : Diffuse Texture
// g_tex_1 : Normalmap Texture
// ==========================
struct VS_STD3D_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_STD3D_OUTPUT
{
    float4 vPosition : SV_Position;
    
    float3 vViewPos : POSITION;
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;  
    float3 vViewBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

VS_STD3D_OUTPUT VS_Std3D(VS_STD3D_INPUT _in)
{
    VS_STD3D_OUTPUT output = (VS_STD3D_OUTPUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vViewTangent  = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal   = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    output.vUV = _in.vUV;
    return output;
}

float4 PS_Std3D(VS_STD3D_OUTPUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (tex_0)
    {        
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }    
    
    float3 vViewNormal = _in.vViewNormal;
    // 노말맵이 있는경우
    if(tex_1)
    {
        float3 vTSNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;        
        vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
        float3x3 matTBN = { _in.vViewTangent, _in.vViewBinormal, _in.vViewNormal};        
        vViewNormal = normalize(mul(vTSNormal, matTBN));
    }    
        
    tLightColor tCol = (tLightColor) 0.f;
        
    for (int i = 0; i < g_iLight3DCount; ++i)
    {
        tLightColor tCurCol = CalLight(i, vViewNormal, _in.vViewPos);
        tCol.vDiff += tCurCol.vDiff;
        tCol.vSpec += tCurCol.vSpec;
        tCol.vAmb += tCurCol.vAmb;
    }
    
    vOutColor.xyz = (tCol.vDiff.xyz * vOutColor.xyz)
                  + tCol.vSpec.xyz
                  + tCol.vAmb.xyz * vOutColor.xyz;
    
    return vOutColor;
}


// =============
// Skybox Shader
// mesh         : sphere
// rasterizer   : CULL_FRONT
// g_tex_0 : Output Texture

// DepthStencilState : Less_Equal
// =============
struct VS_SKY_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_SKY_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_SKY_OUT VS_Skybox(VS_SKY_IN _in)
{
    VS_SKY_OUT output = (VS_SKY_OUT) 0.f; 
    
    float4 vViewPos = mul(float4(_in.vPos, 0.f), g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);
    
    // w 값으로 z 값을 나눌것이기 때문에 미리 w 값을 셋팅해두면
    // 어떤 상황에서도 깊이값이 1.f 로 판정된다.
    vProjPos.z = vProjPos.w * 0.99999f;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Skybox(VS_SKY_OUT _in) : SV_Target
{
    float4 vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);    
    return vOutColor;
}


#endif