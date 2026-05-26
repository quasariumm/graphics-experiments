#include "common.hlsli"

[shader("miss")]
void Miss(inout HitInfo payload : SV_RayPayload)
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);

    float ramp = launchIndex.y / dims.y;
    payload.m_color = float3(0.0, 0.2, 0.7 - 0.3 * ramp);
    payload.m_distance = -1.0;
}