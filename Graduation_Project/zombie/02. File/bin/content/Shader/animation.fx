#ifndef _ANIMATION
#define _ANIMATION

#include "value.fx"

void VectorPermute(uint PermuteX, uint PermuteY, uint PermuteZ, uint PermuteW
    , in float4 V1, in float4 V2
    , out float4 _vOut)
{

    //const uint* aPtr[2];
    //aPtr[0] = &V1;
    //aPtr[1] = &V2;

    float4 aPtr[2] = { V1, V2 };

    float4 Result = (float4) 0.f;
    //auto pWork = &Result;

    const uint i0 = PermuteX & 3;
    const uint vi0 = PermuteX >> 2;
    Result[0] = aPtr[vi0][i0];

    const uint i1 = PermuteY & 3;
    const uint vi1 = PermuteY >> 2;
    Result[1] = aPtr[vi1][i1];

    const uint i2 = PermuteZ & 3;
    const uint vi2 = PermuteZ >> 2;
    Result[2] = aPtr[vi2][i2];

    const uint i3 = PermuteW & 3;
    const uint vi3 = PermuteW >> 2;
    Result[3] = aPtr[vi3][i3];

    _vOut = Result;
}


void MatrixRotationQuaternion(in float4 Quaternion, out matrix _outMat)
{
    float4 Constant1110 = float4(1.f, 1.f, 1.f, 0.f);

    float4 Q0 = Quaternion + Quaternion;
    float4 Q1 = Quaternion * Q0;

    float4 V0 = (float4) 0.f;
    VectorPermute(1, 0, 0, 7, Q1, Constant1110, V0);

    float4 V1 = (float4) 0.f;
    VectorPermute(2, 2, 1, 7, Q1, Constant1110, V1);

    float4 R0 = Constant1110 - V0;
    R0 = R0 - V1;

    V0 = float4(Quaternion[0], Quaternion[0], Quaternion[1], Quaternion[3]);
    V1 = float4(Q0[2], Q0[1], Q0[2], Q0[3]);
    V0 = V0 * V1;

    V1 = float4(Quaternion.w, Quaternion.w, Quaternion.w, Quaternion.w);
    float4 V2 = float4(Q0[1], Q0[2], Q0[0], Q0[3]);
    V1 = V1 * V2;

    float4 R1 = V0 + V1;
    float4 R2 = V0 - V1;

    VectorPermute(1, 4, 5, 2, R1, R2, V0);
    VectorPermute(0, 6, 0, 6, R1, R2, V1);

    matrix M = (matrix) 0.f;
    VectorPermute(0, 4, 5, 3, R0, V0, M._11_12_13_14);
    VectorPermute(6, 1, 7, 3, R0, V0, M._21_22_23_24);
    VectorPermute(4, 5, 2, 3, R0, V1, M._31_32_33_34);
    M._41_42_43_44 = float4(0.f, 0.f, 0.f, 1.f);
    _outMat = M;
}


void MatrixAffineTransformation(in float4 Scaling
    , in float4 RotationOrigin
    , in float4 RotationQuaternion
    , in float4 Translation
    , out matrix _outMat)
{
    matrix MScaling = (matrix) 0.f;
    MScaling._11_22_33 = Scaling.xyz;

    float4 VRotationOrigin = float4(RotationOrigin.xyz, 0.f);
    float4 VTranslation = float4(Translation.xyz, 0.f);

    matrix MRotation = (matrix) 0.f;
    MatrixRotationQuaternion(RotationQuaternion, MRotation);

    matrix M = MScaling;
    M._41_42_43_44 = M._41_42_43_44 - VRotationOrigin;
    M = mul(M, MRotation);
    M._41_42_43_44 = M._41_42_43_44 + VRotationOrigin;
    M._41_42_43_44 = M._41_42_43_44 + VTranslation;
    _outMat = M;
}

struct tFrameTrans
{
    float4 vTranslate;
    float4 vScale;
    float4 qRot;
};

StructuredBuffer<tFrameTrans>   g_arrFrameTrans : register(t10);
StructuredBuffer<matrix>        g_arrOffset : register(t11);
RWStructuredBuffer<matrix>      g_arrFinalMat : register(u0);

// ===========================
// Animation3D Compute Shader
// g_int_0 : Bone Count
// g_int_1 : Current Frame
// g_int_2 : Next Frame
// g_float_0 : Ratio
// ===========================
[numthreads(256, 1, 1)]
void CS_Animation3D(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (g_int_0 <= _iThreadIdx.x)
        return;

    // 오프셋 행렬을 곱하여 최종 본행렬을 만들어낸다.		
    float4 vQZero = float4(0.f, 0.f, 0.f, 1.f);
    matrix matBone = (matrix) 0.f;

    // Frame Data Index == Bone Count * Frame Count + _iThreadIdx.x
    uint iFrameDataIndex = g_int_0 * g_int_1 + _iThreadIdx.x;
    //uint iFrameDataNextIndex = (g_int_0 * g_int_2) + _iThreadIdx.x;

    //float4 vScale = lerp(g_arrFrameTrans[iFrameDataIndex].vScale, g_arrFrameTrans[iFrameDataNextIndex].vScale, g_float_0);
    //float4 qRot = lerp(g_arrFrameTrans[iFrameDataIndex].qRot, g_arrFrameTrans[iFrameDataNextIndex].qRot, g_float_0);
    //float4 vTranslate = lerp(g_arrFrameTrans[iFrameDataIndex].vTranslate, g_arrFrameTrans[iFrameDataNextIndex].vTranslate, g_float_0);

    //MatrixAffineTransformation(vScale, vQZero, qRot, vTranslate, matBone);

    MatrixAffineTransformation(g_arrFrameTrans[iFrameDataIndex].vScale
        , vQZero, g_arrFrameTrans[iFrameDataIndex].qRot
        , g_arrFrameTrans[iFrameDataIndex].vTranslate
        , matBone);

    matrix matOffset = transpose(g_arrOffset[_iThreadIdx.x]);
    g_arrFinalMat[_iThreadIdx.x] = mul(matOffset, matBone);
}

#endif
