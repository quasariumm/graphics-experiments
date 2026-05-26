#include "common.hlsli"

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    float3 barycentrics =
        float3(1.0 - attrib.m_bary.x - attrib.m_bary.y, attrib.m_bary.x, attrib.m_bary.y);

    uint vertId = 3 * PrimitiveIndex();
    float3 hitColor = barycentrics;

    payload.m_color = hitColor;
    payload.m_distance = RayTCurrent();
}