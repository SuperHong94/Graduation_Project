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



#endif