#include "shader_resources.hlsli"

struct PSIn
{
    float2 Uv0 : TEXCOORD0;
    float2 Uv1 : TEXCOORD1;
    float3 WorldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float3 CurClip : CURCLIP;
    float3 PrevClip : PREVCLIP;
    float4 Position : SV_Position;
};

ConstantBuffer<ShaderMaterial> MaterialC : register(b1);

SamplerState SSLinearWrap : register(s0);

float4 main(PSIn IN) : SV_Target
{
    float3 color = 0.0;

    if (MaterialC.m_texIndices1[TEX_ALBEDO] != -1)
    {
        Texture2D<float4> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_ALBEDO]];
        color = tex.Sample(SSLinearWrap, IN.Uv0).rgb;
    }
    if (MaterialC.m_texIndices1[TEX_EMISSIVE] != -1)
    {
        Texture2D<float4> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_EMISSIVE]];
        color += tex.Sample(SSLinearWrap, IN.Uv0).rgb;
    }

    return float4(color, 1.0);
}