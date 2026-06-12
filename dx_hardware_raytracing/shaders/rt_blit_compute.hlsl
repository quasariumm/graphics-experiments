#include "tonemap.hlsli"

struct Params 
{
    int m_input;
    int m_output;
    uint2 m_size;
};
ConstantBuffer<Params> Params32C : register(b0);

[numthreads(16, 16, 1)]
void main(uint2 tId : SV_DispatchThreadID)
{
    if (any(tId >= Params32C.m_size)) return;

    RWTexture2D<float4> Input = ResourceDescriptorHeap[Params32C.m_input];
    RWTexture2D<float4> Output = ResourceDescriptorHeap[Params32C.m_output];
    Output[tId] = float4(TonemappingAgX(Input[tId].rgb), 1.0);
}
