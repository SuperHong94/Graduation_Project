#ifndef _COMPUTE
#define _COMPUTE
#include "value.fx"

RWTexture2D<float4> g_rwtex_0 : register(u0);

// Dispatch(1, 1024, 1)
[numthreads(1024, 1, 1)] // 1024 -> Group, hlsl 5.0 기준 그룹당 스레드 최대 개수는 1024
void CS_TEST(int3 _iThreadIdx : SV_DispatchThreadID)
{
    //g_rwtex_0[_iThreadIdx.xy] = float4(1.f, 0.f, 0.f, 1.f);
    
    //if (_iThreadIdx.y == 512)
    //    g_rwtex_0[_iThreadIdx.xy] = float4(1.f, 0.f, 0.f, 1.f);
    //else
    //    g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 0.f, 1.f);
    
    if (g_int_0)
    {
        if (_iThreadIdx.y == 512)
            g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 1.f, 0.f, 1.f);
        else
            g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 0.f, 1.f);
    }
    else
    {
        if (_iThreadIdx.y == 512)
            g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 1.f, 1.f);
        else
            g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 0.f, 1.f);
    }
}


StructuredBuffer<matrix> g_arrSrc_0 : register(t8);
StructuredBuffer<matrix> g_arrSrc_1 : register(t9);
StructuredBuffer<matrix> g_arrSrc_2 : register(t10);
StructuredBuffer<matrix> g_arrSrc_3 : register(t11);
StructuredBuffer<matrix> g_arrSrc_4 : register(t12);

RWStructuredBuffer<matrix> g_arrFinalMat : register(u0);
// ======================
// BoneMatrix 복사 쉐이더
// g_int_0 : Bone Count
// g_int_1 : Row Index
// g_int_2 : Max Row
// g_arrFinelMat(u0) : 복사받을 목적지 버퍼
// g_arrOffset(t11)  : 원본 버퍼
// ======================
[numthreads(256, 1, 1)]
void CS_CopyBoneMatrix(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (_iThreadIdx.x >= g_int_0)
        return;
        
    if (g_int_1 + 0 >= g_int_2)
        return;        
    g_arrFinalMat[g_int_0 * (g_int_1 + 0) + _iThreadIdx.x] = g_arrSrc_0[_iThreadIdx.x];
     
    if (g_int_1 + 1 >= g_int_2)
        return;
    g_arrFinalMat[g_int_0 * (g_int_1 + 1) + _iThreadIdx.x] = g_arrSrc_1[_iThreadIdx.x];
    
    if (g_int_1 + 2 >= g_int_2)
        return;
    g_arrFinalMat[g_int_0 * (g_int_1 + 2) + _iThreadIdx.x] = g_arrSrc_2[_iThreadIdx.x];
    
    if (g_int_1 + 3 >= g_int_2)
        return;
    g_arrFinalMat[g_int_0 * (g_int_1 + 3) + _iThreadIdx.x] = g_arrSrc_3[_iThreadIdx.x];
    
    if (g_int_1 + 4 >= g_int_2)
        return;
    g_arrFinalMat[g_int_0 * (g_int_1 + 4) + _iThreadIdx.x] = g_arrSrc_4[_iThreadIdx.x];
}

#endif