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

#endif