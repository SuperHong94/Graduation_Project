#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 vDiffuse : SV_TARGET;
    float4 vSpecular : SV_TARGET1;
};

// ========================
// Directional Light Shader
// ========================
// mesh : RectMesh
// blendstate : One-One
// depthstencilstate : No Depth check, No Depth Write
//
// g_int_0 : Light Index

// g_tex_0 : Normal Target
// g_tex_1 : Position Target
// g_tex_2 : 
// g_tex_3 : ShadowMap Target

// g_mat_0 : Light VP(View, Proj)
//==========================
VS_OUTPUT VS_DirLight(VS_INPUT _in)
{
    VS_OUTPUT output = (VS_OUTPUT)0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

PS_OUTPUT PS_DirLight(VS_OUTPUT _in)
{
    PS_OUTPUT output = (PS_OUTPUT)0.f;

    float3 vViewPos = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
    if (vViewPos.z <= 1.f)
    {
        clip(-1);
    }

    float3 vViewNormal = g_tex_0.Sample(g_sam_0, _in.vUV).xyz;

    tLightColor tCurCol = CalLight(g_int_0, vViewNormal, vViewPos);

    // 그림자 판정    
    // 빛이 없으면 그림자 처리를 하지 않는다.
    if (dot(tCurCol.vDiff, tCurCol.vDiff) != 0.f)
    {
        float4 vWorldPos = mul(float4(vViewPos.xyz, 1.f), g_matViewInv); // 메인카메라 view 역행렬을 곱해서 월드좌표를 알아낸다.
        float4 vShadowProj = mul(vWorldPos, g_mat_0); // 광원 시점으로 투영시킨 좌표 구하기
        float fDepth = vShadowProj.z / vShadowProj.w; // w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)

        // 계산된 투영좌표를 UV 좌표로 변경해서 ShadowMap 에 기록된 깊이값을 꺼내온다.
        float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
            , (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

        if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
            && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
        {
            float fShadowDepth = g_tex_3.Sample(g_sam_0, vShadowUV).r;

            // 그림자인 경우 빛을 약화시킨다.
            if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
            {
                tCurCol.vDiff *= 0.1f;
                tCurCol.vSpec = (float4) 0.f;
            }
        }
    }


    output.vDiffuse = tCurCol.vDiff + tCurCol.vAmb;
    output.vSpecular = tCurCol.vSpec;

    return output;
}


// ========================
// Point Light Shader
// ========================
// mesh : SphereMesh
// blendstate : One-One
// depthstencilstate : No Depth check, No Depth Write

// g_int_0 : Light Index
// g_tex_0 : Normal Target
// g_tex_1 : Position Target

// g_vec2_0 : RenderTarget Resolution
// ===================================

VS_OUTPUT VS_PointLight(VS_INPUT _in)
{
    VS_OUTPUT output = (VS_OUTPUT)0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    return output;
}


PS_OUTPUT PS_PointLight(VS_OUTPUT _in)
{
    PS_OUTPUT output = (PS_OUTPUT)0.f;

    // Screen 좌표
    float2 vFullUV = float2(_in.vPosition.x / g_vec2_0.x, _in.vPosition.y / g_vec2_0.y);
    float3 vViewPos = g_tex_1.Sample(g_sam_0, vFullUV).xyz;

    if (vViewPos.z <= 0.f)
    {
        //output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
        //return output;
        clip(-1);
    }

    // Sphere 내부에 들어오는 정점인지 확인한다.
    float4 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv);
    float4 vLocalPos = mul(vWorldPos, g_matWorldInv);
    if (length(vLocalPos.xyz) > 1.f)
    {
        //output.vDiffuse = float4(0.f, 1.f, 0.f, 1.f);
        //return output;

        clip(-1);
    }

    float3 vViewNormal = g_tex_0.Sample(g_sam_0, vFullUV).xyz;

    tLightColor tCurCol = CalLight(g_int_0, vViewNormal, vViewPos);

    output.vDiffuse = tCurCol.vDiff + tCurCol.vAmb;
    output.vSpecular = tCurCol.vSpec;

    return output;
}


// ============
// Merge Lights
// ============
// mesh : RectMesh
// blendstate : default
// depthstencilstate : No Depth check, No Depth Write
//
// g_tex_0 : Diffuse Target
// g_tex_1 : Light Target
// g_tex_2 : Specular Target
//==========================
VS_OUTPUT VS_MergeLight(VS_INPUT _in)
{
    VS_OUTPUT output = (VS_OUTPUT)0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_MergeLight(VS_OUTPUT _in) : SV_Target
{
    float4 vLightPow = g_tex_1.Sample(g_sam_0, _in.vUV);
    if (vLightPow.x == 0.f && vLightPow.y == 0.f && vLightPow.z == 0.f)
    {
        clip(-1);
    }

    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    float4 vSpec = g_tex_2.Sample(g_sam_0, _in.vUV);

    return (vColor * vLightPow) + vSpec;
}

// =================
// Shadow Map Shader
// =================
struct VS_ShadowIn
{
    float3 vPos : POSITION;
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_ShadowOut
{
    float4 vPos : SV_Position;
    float4 vProj : POSITION;
};

VS_ShadowOut VS_ShadowMap(VS_ShadowIn _in)
{
    VS_ShadowOut output = (VS_ShadowOut)0.f;

    if (g_int_0)
    {
        Skinning(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }

    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProj = output.vPos;

    return output;
}

float4 PS_ShadowMap(VS_ShadowOut _input) : SV_Target
{
    return float4(_input.vProj.z / _input.vProj.w, 0.f, 0.f, 0.f);
}
#endif
