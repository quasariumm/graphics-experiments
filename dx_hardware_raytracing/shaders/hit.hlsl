#include "materials.hlsli"
#include "pbr.hlsli"
#include "lighting.hlsli"
#include "ray_bounce.hlsli"

struct SceneConstBuffer
{
    int m_vertexBuffers;
    int m_indexBuffers;
    int m_materialsBuffers;
    int m_materialIndicesBuffers;
    int m_blasGeometryCounts;
    uint m_debugMode;
    uint m_maxRecursionDepth;
    uint m_frameNum;
    uint4 m_morePadding[14];
};
ConstantBuffer<SceneConstBuffer> SceneCB : register(b0);


RaytracingAccelerationStructure SceneBVH : register(t0);


SamplerState SSLinearWrap : register(s0);


Vertex GetFragmentData(in StructuredBuffer<Vertex> vertexBuffer, in Buffer<uint> indexBuffer, float3 bary) 
{
    uint offset = 3 * PrimitiveIndex();
    Vertex v0 = vertexBuffer[indexBuffer[offset]];
    Vertex v1 = vertexBuffer[indexBuffer[offset + 1]];
    Vertex v2 = vertexBuffer[indexBuffer[offset + 2]];

    Vertex fragment;
    fragment.Position = bary.x * v0.Position + bary.y * v1.Position + bary.z * v2.Position;
    fragment.Uv0 = bary.x * v0.Uv0 + bary.y * v1.Uv0 + bary.z * v2.Uv0;
    fragment.Uv1 = bary.x * v0.Uv1 + bary.y * v1.Uv1 + bary.z * v2.Uv1;
    fragment.Normal = bary.x * v0.Normal + bary.y * v1.Normal + bary.z * v2.Normal;
    fragment.Tangent = bary.x * v0.Tangent + bary.y * v1.Tangent + bary.z * v2.Tangent;

    return fragment;
}

static const uint shadow_flags = RAY_FLAG_FORCE_OPAQUE
                                    | RAY_FLAG_CULL_BACK_FACING_TRIANGLES 
                                    | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER 
                                    | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    if (GetIsShadowRay(payload.m_flags))
        return;

    float3 barycentrics =
        float3(1.0 - attrib.m_bary.x - attrib.m_bary.y, attrib.m_bary.x, attrib.m_bary.y);
    
    // Set distance on primary ray only
    if (all(payload.m_color == 0.0))
        payload.m_distance = RayTCurrent();

    // Get the triangle
    if (SceneCB.m_vertexBuffers == -1 || SceneCB.m_indexBuffers == -1)
        return;
    
    Buffer<uint> blasGeometryCounts = ResourceDescriptorHeap[SceneCB.m_blasGeometryCounts];
    uint geomIndex = InstanceIndex() * blasGeometryCounts[InstanceIndex()] + GeometryIndex();
    
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[SceneCB.m_vertexBuffers + geomIndex];
    Buffer<uint> indices = ResourceDescriptorHeap[SceneCB.m_indexBuffers + geomIndex];

    // Get the interpolated vertex data
    Vertex fragment = GetFragmentData(vertices, indices, barycentrics);

    // Shade with the material
    StructuredBuffer<ShaderMaterial> materials = ResourceDescriptorHeap[SceneCB.m_materialsBuffers + InstanceIndex()];
    Buffer<int> materialIndices = ResourceDescriptorHeap[SceneCB.m_materialIndicesBuffers + InstanceIndex()];

    int materialIndex = materialIndices[GeometryIndex()];

    ShaderMaterial material;
    if (materialIndex == -1)
        material = (ShaderMaterial)0;
    else
        material = materials[materialIndex];

    FragmentAttributes attributes = GetFragmentAttributes(SSLinearWrap, fragment, material);

    ShaderLight keyLight = CreateDirectionalLight(
        normalize(float3(-1.0, -1.0, 0.5)),
        1.0,
        1.0
    );

    // Perform PBR
    float3 hitPos = WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
    
    float3 wo = -WorldRayDirection();
    
    // Temporarily only sample key light
    float3 wi = -keyLight.m_direction;

    float3 lightSample = SampleLight(keyLight, hitPos, attributes.m_normal);

    // Shadow ray
    // RayDesc shadowRay = ShadowRay(attributes, hitPos, wi);
    // HitInfo shadowPayload = payload;
    // SetIsShadowRay(shadowPayload.m_flags);
    // TraceRay(
    //     SceneBVH,
    //     shadow_flags,
    //     0xff,
    //     0u,
    //     0u,
    //     0u, // TODO: Change this to a more dedicated miss shader
    //     shadowRay,
    //     shadowPayload
    // );

    // if (!GetHasMissed(shadowPayload.m_flags)) // Hit something
    //     lightSample = 0.0;

    // Get the BRDF/PDF
    float3 brdf = ComputeBRDF(
        attributes.m_normal,
        wo, wi,
        attributes.m_albedo.rgb,
        attributes.m_roughness,
        attributes.m_metallic
    );

    float pdf = ComputePDF(
        attributes.m_normal,
        wo, wi,
        attributes.m_roughness
    );

    // Debug mode
    if (SceneCB.m_debugMode != debug_none)
    {
        if (SceneCB.m_debugMode == debug_brdf)
        {
            payload.m_color = brdf;
            return;
        }
        if (SceneCB.m_debugMode == debug_pdf)
        {
            payload.m_color = pdf;
            return;
        }
        payload.m_color = GetDebugOutput(fragment, attributes, SceneCB.m_debugMode);
        return;
    }

    // Update the ray color
    if (pdf != 0.0)
        payload.m_color += payload.m_rayColor * lightSample * brdf / pdf;
    
    payload.m_color += payload.m_rayColor * attributes.m_emissive;

    payload.m_rayColor *= attributes.m_albedo.rgb;

    // Update recursion depth and return if done
    IncRecursionDepth(payload.m_flags);
    if (GetRecursionDepth(payload.m_flags) >= SceneCB.m_maxRecursionDepth)
        return;
    
    // Skip when the albedo is pitch black
    if (all(payload.m_rayColor < 0.0001))
        return;

    // Trace a new ray
    RayDesc newRay = BounceRay(
        payload, hitPos, 
        wo, attributes, 
        SceneCB.m_frameNum
    );
    
    HitInfo newPayload = payload;
    
    TraceRay(
        SceneBVH, 
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
        0xff,
        0u,
        0u,
        0u,
        newRay,
        newPayload
    );

    payload = newPayload;
}