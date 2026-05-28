#include "shader_resources.hlsli"

struct VSIn
{
    float3 Position : POSITION;
    float _padding1 : PADDING;
    float2 Uv0 : UV_PRIM;
    float2 Uv1 : UV_SEC;
    float3 Normal : NORMAL;
    float _padding2 : PADDING_TWO;
    float4 Tangent : TANGENT;
};

struct VSOut
{
    float2 Uv0 : TEXCOORD0;
    float2 Uv1 : TEXCOORD1;
    float3 WorldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 Position : SV_Position;
};

ConstantBuffer<CameraConstBuffer> CameraCB : register(b0);
ConstantBuffer<ShaderTransform> TransformC : register(b2);

VSOut main(VSIn IN) 
{
    VSOut OUT;
    
    float4 worldPos = mul(TransformC.m_worldMatrix, float4(IN.Position, 1.0f));
    OUT.Position = mul(CameraCB.m_shaderCamera.m_viewProjectionMatrix, worldPos);
    OUT.WorldPos = worldPos.xyz;
    
    OUT.Uv0 = IN.Uv0;
    OUT.Uv1 = IN.Uv1;
    
    OUT.Normal = mul(TransformC.m_normalMatrix, float4(IN.Normal, 0.0f)).xyz;
    
    OUT.Tangent.xyz = mul(TransformC.m_normalMatrix, float4(IN.Tangent.xyz, 0.0f)).xyz;
    OUT.Tangent.w = IN.Tangent.w;
    
    return OUT;
}