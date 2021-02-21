#include "value.fx"
#include "func.fx"

#ifndef _STD3D
#define _STD3D

// ==========================
// Std3D Shader
// Deferred = true
//
// g_tex_0 : Diffuse Texture
// g_tex_1 : Normalmap Texture
// BlendState : false
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
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV).xyz);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_STD3D_OUTPUT
{
    float4 vTarget0 : SV_Target0; // Diffuse
    float4 vTarget1 : SV_Target1; // Normal
    float4 vTarget2 : SV_Target2; // Position
};

PS_STD3D_OUTPUT PS_Std3D(VS_STD3D_OUTPUT _in)
{
    PS_STD3D_OUTPUT output = (PS_STD3D_OUTPUT) 0.f;
    
    if (tex_0)
        output.vTarget0 = g_tex_0.Sample(g_sam_0, _in.vUV);
    else
        output.vTarget0 = float4(1.f, 0.f, 1.f, 1.f);
        
    float3 vViewNormal = _in.vViewNormal;
    // 노말맵이 있는경우
    if (tex_1)
    {
        float3 vTSNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
        float3x3 matTBN = { _in.vViewTangent, _in.vViewBinormal, _in.vViewNormal };
        vViewNormal = normalize(mul(vTSNormal, matTBN));
    }
    
    output.vTarget1.xyz = vViewNormal;
    output.vTarget2.xyz = _in.vViewPos;
    
    return output;
}


// =============
// Skybox Shader
// mesh         : sphere
// rasterizer   : CULL_FRONT
// DepthStencilState : Less_Equal
// g_tex_0 : Output Texture
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
    vProjPos.z = vProjPos.w;
    
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