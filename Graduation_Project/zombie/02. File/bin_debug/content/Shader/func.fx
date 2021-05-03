#ifndef _FUNC
#define _FUNC

#include "value.fx"

tLightColor CalLight(int _iLightIdx, float3 _vViewNormal, float3 _vViewPos)
{
    tLightColor tCol = (tLightColor) 0.f;
    
      
    float3 vViewLightDir = (float3) 0.f;
    float fDiffPow = 0.f;
    float fSpecPow = 0.f;
    float fRatio = 1.f;
    
    // Directional Light
    if (g_Light3D[_iLightIdx].iLight3DType == 0)
    {        
        // 광원의 방향   
        vViewLightDir = normalize(mul(float4(g_Light3D[_iLightIdx].vLightDir.xyz, 0.f), g_matView).xyz);
        fDiffPow = saturate(dot(-vViewLightDir, _vViewNormal));      
    }
    // Point Light
    else if (g_Light3D[_iLightIdx].iLight3DType == 1)
    {
        float3 vViewLightPos = mul(float4(g_Light3D[_iLightIdx].vLightPos.xyz, 1.f), g_matView).xyz;        
        vViewLightDir = normalize(_vViewPos - vViewLightPos);
        
        fDiffPow = saturate(dot(-vViewLightDir, _vViewNormal));
        
        // Ratio 계산
        float fDistance = distance(_vViewPos, vViewLightPos);
        if (0.f == g_Light3D[_iLightIdx].fRange)
            fRatio = 0.f;
        else
            fRatio = saturate(1.f - fDistance / g_Light3D[_iLightIdx].fRange);        
    }
    // Spot Light
    else
    {
        
    }    
    
    // 반사 방향
    float3 vReflect = normalize(vViewLightDir + 2 * (dot(-vViewLightDir, _vViewNormal) * _vViewNormal));    
    float3 vEye = normalize(_vViewPos);
    fSpecPow = saturate(dot(-vEye, vReflect));    
    fSpecPow = pow(fSpecPow, 10);
 
    tCol.vDiff = fDiffPow * g_Light3D[_iLightIdx].tCol.vDiff * fRatio;
    tCol.vSpec = fSpecPow * g_Light3D[_iLightIdx].tCol.vSpec * fRatio;
    tCol.vAmb = g_Light3D[_iLightIdx].tCol.vAmb;
    
    return tCol;
}


static float gaussian5x5[25] =
{
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
};

static float gaussian3x3[9] =
{
    0.035f, 0.16f, 0.035f,
    0.16f, 0.22f, 0.16f,
    0.035f, 0.16f, 0.035f,
};

float4 gaussian3x3Sample(in int2 _uv, in RWTexture2D<float4> _tex)
{
    float4 fOut = (float4) 0.f;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            fOut += _tex[_uv + int2(i - 1, j - 1)] * gaussian3x3[i * 3 + j];
        }
    }
    return fOut;
}


float gaussian5x5Sample(in int2 _uv, in Texture2D _tex)
{
    float4 fOut = (float4) 0.f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            fOut += _tex[_uv + int2(i - 2, j - 2)] * gaussian5x5[i * 5 + j];
        }
    }
    return fOut.x;
}

#endif