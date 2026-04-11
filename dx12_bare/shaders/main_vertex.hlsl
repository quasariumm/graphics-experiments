#include "shader_resources.hlsli"

ConstantBuffer<CameraConstBuffer> CameraCB : register(b0);

struct VSIn
{
    float3 Position : POSITION;
    float2 Uv0 : UV_PRIM;
    float2 Uv1 : UV_SEC;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
};

struct VSOut
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

VSOut main(VSIn IN) 
{
    VSOut OUT;
    
    float4 worldPos = float4(IN.Position, 1.0f);
    OUT.Position = mul(CameraCB.m_shaderCamera.m_viewProjectionMatrix, worldPos);
    OUT.WorldPos = worldPos.xyz;
    
    OUT.Uv0 = IN.Uv0;
    OUT.Uv1 = IN.Uv1;
    
    OUT.Normal = IN.Normal;
    
    OUT.Tangent.xyz = IN.Tangent.xyz;
    OUT.Tangent.w = IN.Tangent.w;
    
    // Velocity calculation
    OUT.CurClip = float3(OUT.Position.xy, OUT.Position.w);
    float4 prevClip = mul(CameraCB.m_shaderCamera.m_prevViewProjectionMatrix, worldPos);
    OUT.PrevClip = float3(prevClip.xy, prevClip.w);
    
    return OUT;
}