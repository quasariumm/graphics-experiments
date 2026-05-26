#include "common.hlsli"

RWTexture2D<float4> Output : register(u0);

RaytracingAccelerationStructure SceneBVH : register(t0);

ConstantBuffer<ShaderCamera> CameraCB : register(b0);

[shader("raygeneration")]
void RayGen() 
{
    // Initialise a default payload
    HitInfo payload;
    payload.m_color = 0.0;
    payload.m_distance = 0.0;

    // Get the thread ID
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 uv = (((launchIndex.xy + 0.5) / dims.xy) * 2.0 - 1.0);
    uv.y *= -1.0;

    // Get the direction based on the camera
    float4 target = mul(CameraCB.m_inverseViewProjection, float4(uv, 1.0, 1.0));
    target /= target.w;
    
    // Make the ray struct
    RayDesc ray;
    ray.Origin = CameraCB.m_cameraPosition;
    ray.Direction = normalize(target.xyz - CameraCB.m_cameraPosition);
    ray.TMin = 0.001;
    ray.TMax = 10000.0;

    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xff, 0, 0, 0, ray, payload);

    Output[launchIndex] = float4(payload.m_color, 1.0);
}