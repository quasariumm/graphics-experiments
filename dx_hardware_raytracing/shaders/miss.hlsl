#include "common.hlsli"

[shader("miss")]
void Miss(inout HitInfo payload)
{
    if (GetIsShadowRay(payload.m_flags))
    {
        SetHasMissed(payload.m_flags);
        return;
    }

    float3 rd = WorldRayDirection();

    // Sky gradient
    float t = saturate(rd.y * 0.5f + 0.5f);
    float3 horizon = float3(0.8f, 0.9f, 1.0f);
    float3 zenith  = float3(0.1f, 0.3f, 0.8f);
    float3 sky     = lerp(horizon, zenith, pow(t, 0.6f));

    // Sun
    float3 sunDir  = normalize(float3(0.3f, 0.6f, -0.5f));
    float  sun     = pow(saturate(dot(rd, sunDir)), 512.0f);
    float  glow    = pow(saturate(dot(rd, sunDir)), 8.0f) * 0.3f;
    sky += float3(1.0f, 0.9f, 0.6f) * (sun + glow);

    // Ground (below horizon)
    sky = lerp(float3(0.15f, 0.12f, 0.1f), sky, smoothstep(-0.05f, 0.05f, rd.y));

    if (GetRecursionDepth(payload.m_flags) == 0)
        payload.m_color = sky;
    else
        payload.m_color += payload.m_rayColor * sky;
    payload.m_distance = -1.0f;
}