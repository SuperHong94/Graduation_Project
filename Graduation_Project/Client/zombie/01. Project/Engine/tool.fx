#include "value.fx"

#ifndef _TOOL
#define _TOOl

struct VS_DIR_OUTPUT
{
    float4 vOutPos : SV_Position;
    float4 vOutColor : COLOR;
};

// ======================
// Object Dir Shader
// Topology : Line Strip
// ======================
VS_DIR_OUTPUT VS_Dir(float3 _vPos : POSITION, float4 _vColor : COLOR)
{
    VS_DIR_OUTPUT output = (VS_DIR_OUTPUT) 0;
    
    // 투영좌표계를 반환할 때에는 float4 4번째 w 요소에 1.f 을 넣어준다.
    float4 vWorldPos = mul(float4(_vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vOutPos = vProjPos;
    output.vOutColor = _vColor;

    return output;
}

float4 PS_Dir(VS_DIR_OUTPUT _input) : SV_Target
{
    return _input.vOutColor;
}


// ======================
// Grid Shader
// AlphaBlend : true
// g_int_0 : 간격 단계
// g_float_0 : 카메라 높이에 따른 그리드의 투명도
// g_float_1 : 선의 두께
// g_vec4_0  : 카메라 위치
// g_vec4_1  : Grid Color
// g_tex_0 : Position Target
// ======================
struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
    float3 vViewPos : POSITION1;
};

VTX_OUT VS_Grid(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vUV = _in.vUV;

    return output;
}

float4 PS_Grid(VTX_OUT _in) : SV_Target
{
    //float2 vFullUV = float2(_in.vPosition.x / g_vResolution.x, _in.vPosition.y / g_vResolution.y);
    //float3 vTargetViewPos = g_tex_0.Sample(g_sam_0, vFullUV).xyz;
    
    //float fLen = length(vTargetViewPos);    
    //float fMax = fLen * 0.01f; // 거리값의 10%를 최대보간값으로 본다.    
    
    //float fDepthRatio = 1.f;
    //if (vTargetViewPos.z != 0.f)
    //{        
    //    fDepthRatio = 1.f - saturate((length(_in.vViewPos) - length(vTargetViewPos)) / fMax);
    //}
    
    
    // 현재 월드 위치값이 카메라로부터 멀면    
    float fDist = abs(distance(_in.vWorldPos, g_vec4_0.xyz));
    float fThickness = g_float_1 * (fDist / abs(g_vec4_0.y)) * 2.f;

    // 현재보다 다음 단계 간격을 잡아낸다.
    int iStep = 1;
    for (int i = 0; i < g_int_0 + 1; ++i)
    {
        iStep *= 10;
    }

    if (fDist > 400.f && fDist > abs(g_vec4_0.y) * 7.f)
    {
        // 시야 멀어서 안보임
        clip(-1);
    }

    float fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    float fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    float fXRatio = 0.f;
    float fZRatio = 0.f;
    float fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(g_vec4_1.rgb, fRatio); // * fDepthRatio);
    }
    
    // 현재 단계 라인
    iStep = 1;
    for (i = 0; i < g_int_0; ++i)
    {
        iStep *= 10;
    }

    fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    fXRatio = 0.f;
    fZRatio = 0.f;
    fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(g_vec4_1.rgb, fRatio * g_float_0); // * fDepthRatio);
    }
    
    clip(-1);
    return float4(0.f, 0.f, 0.f, 0.f);
}




#endif