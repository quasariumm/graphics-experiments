#include "shader_resources.hlsli"

ConstantBuffer<CameraConstBuffer> CameraCB : register(b0);

struct VSIn
{
    float4 Position : POSITION;
};

struct VSOut
{
    float4 Position : SV_Position;
};

VSOut main(VSIn IN) 
{
    VSOut OUT;
    OUT.Position = mul(CameraCB.m_shaderCamera.m_viewProjectionMatrix, IN.Position);
    return OUT;
}