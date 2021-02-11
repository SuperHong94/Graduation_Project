#ifndef _VALUE
#define _VALUE


// struct
struct tLight2DInfo
{
    float4 vLightPos;
    float4 vLightColor;
    float4 vLightDir;
    int iLight2DType;
    float fRange;
    float fAngle;
    int ipadding;
};

struct tLightColor
{
    float4 vDiff;
    float4 vSpec;
    float4 vAmb;
};

struct tLight3DInfo
{
    tLightColor tCol;
    float4      vLightPos;   
    float4      vLightDir;
    int         iLight3DType;
    float       fRange;
    float       fAngle;
    int         ipadding;
};


// constant register 
cbuffer TRANSFORM_MATRIX : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;

    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL_PARAM : register(b1)
{
    float4 g_vDiff;
    float4 g_vSpec;
    float4 g_vEmv;

    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;

    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;

    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;

    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;

    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;

    int tex_0;
    int tex_1;
    int tex_2;
    int tex_3;
};

cbuffer ANIM2D : register(b2)
{
    float2  g_vLT;   // UV 좌상단
    float2  g_vLen;  // UV 길이
    float2  g_vOffset; // Offset
    int     IsAnim2D;   // Animation2D 컴포넌트 사용여부
    int     padding2;
}

cbuffer LIGHT2D : register(b3)
{
    tLight2DInfo g_Light2D[100];
    int         g_iLight2DCount;
    int3        padding3;
};

cbuffer LIGHT3D : register(b4)
{
    tLight3DInfo    g_Light3D[100];
    int             g_iLight3DCount;
    int3            padding3DLight;
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

Texture2DArray g_arrtex_0 : register(t8);
Texture2DArray g_arrtex_1 : register(t9);
Texture2DArray g_arrtex_2 : register(t10);
Texture2DArray g_arrtex_3 : register(t11);

Texture2D g_tex_anim2D : register(t12);

// Sampler
SamplerState g_sam_0 : register(s0); // anisotrophic
SamplerState g_sam_1 : register(s1); // point


int HasTex(in Texture2D _tex)
{
    uint iWidth = 0;
    uint iHeight = 0;
    _tex.GetDimensions(iWidth, iHeight);

    if (iWidth == 0 || iHeight == 0)
    {
        return 0;
    }
    return 1;
}

#endif