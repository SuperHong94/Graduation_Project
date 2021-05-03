#include "value.fx"
#include "func.fx"

#ifndef _TESS
#define _TESS



// ====================
// Tessellation Shader
// ====================
// mesh : rect mesh
// rasterizer : wireframe
struct PatchTess
{
    float fInsideTess : SV_InsideTessFactor;
    float arrEdgeTess[3] : SV_TessFactor;
};

struct VS_TESS
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Tess(VS_TESS _in)
{
    VS_OUT output = _in;
        
    return output;
}

// Hull Shader
// 패치 상수 함수
PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> _input, int _iPatchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess) 0.f;
        
    output.arrEdgeTess[0] = 4.f;
    output.arrEdgeTess[1] = 3.f;
    output.arrEdgeTess[2] = 2.f;
    output.fInsideTess = 4.f;
        
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc")]
VS_TESS HS_Tess(InputPatch<VS_OUT, 3> _input, int _iVtxIdx : SV_OutputControlPointID, int _iPatchID : SV_PrimitiveID)
{
    VS_TESS output = (VS_TESS) 0.f;
    
    output.vPos = _input[_iVtxIdx].vPos;
    output.vUV = _input[_iVtxIdx].vUV;
    
    return output;
}

// --> Tessellator

// Domain Shader
struct DS_OUTPUT_TESS
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DS_OUTPUT_TESS DS_Tess(const OutputPatch<VS_TESS, 3> _in, float3 _vLocation : SV_DomainLocation, PatchTess _patch)
{
    DS_OUTPUT_TESS output = (DS_OUTPUT_TESS) 0.f;
    
    float3 vLocalPos = _in[0].vPos * _vLocation[0] + _in[1].vPos * _vLocation[1] + _in[2].vPos * _vLocation[2];
    float2 vUV = _in[0].vUV * _vLocation[0] + _in[1].vUV * _vLocation[1] + _in[2].vUV * _vLocation[2];
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    return output;
}

// --> Raterizer

float4 PS_Tess(DS_OUTPUT_TESS _in) : SV_Target
{
    return float4(1.f, 0.f, 1.f, 1.f);
}


#endif