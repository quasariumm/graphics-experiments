#include "meshlets.hlsli"

float4 main(MSOut IN) : SV_Target 
{
    return float4(IN.Color, 1.0);
}