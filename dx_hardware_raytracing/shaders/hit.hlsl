#include "common.hlsli"

struct GeometryBufferHeapIndices
{
    int m_vertexBuffers;
    int m_indexBuffers;
    uint2 m_padding;
    uint4 m_morePadding[15];
};
ConstantBuffer<GeometryBufferHeapIndices> Geometry32C : register(b0);

StructuredBuffer<ShaderMaterial> MaterialsCB : register(t0);

SamplerState SSLinearWrap : register(s0);

Vertex GetFragmentData(in StructuredBuffer<Vertex> vertexBuffer, in StructuredBuffer<uint> indexBuffer, float3 bary) 
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

    // Debug
    fragment.Position = float3(indexBuffer[offset] % 64 / 64.0, indexBuffer[offset + 1] % 64 / 64.0, indexBuffer[offset + 2] % 64 / 64.0);

    return fragment;
}

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    float3 barycentrics =
        float3(1.0 - attrib.m_bary.x - attrib.m_bary.y, attrib.m_bary.x, attrib.m_bary.y);
    
    // Get the triangle
    if (Geometry32C.m_vertexBuffers == -1 || Geometry32C.m_indexBuffers == -1)
        return;
    
    StructuredBuffer<Vertex> vertices = ResourceDescriptorHeap[Geometry32C.m_vertexBuffers + InstanceIndex()];
    StructuredBuffer<uint> indices = ResourceDescriptorHeap[Geometry32C.m_indexBuffers + InstanceIndex()];

    // Get the interpolated vertex data
    Vertex fragment = GetFragmentData(vertices, indices, barycentrics);

    // Shade with the material
    ShaderMaterial material = MaterialsCB[InstanceIndex()];

    float3 color = 0.0;

    if (material.m_texIndices1[TEX_ALBEDO] != -1)
    {
        Texture2D<float4> albedoTex = ResourceDescriptorHeap[material.m_texIndices1[TEX_ALBEDO]];
        color = albedoTex.SampleLevel(SSLinearWrap, fragment.Uv0, 0.0).rgb;
    }

    float3x4 objectToWorld = ObjectToWorld3x4();
    // For normals, use inverse transpose (= transpose of inverse)
    // For uniform scale, the 3x3 upper-left works fine
    float3 worldNormal = normalize(mul((float3x3)objectToWorld, fragment.Normal));
    
    payload.m_color = worldNormal * 0.5 + 0.5;
    payload.m_color = fragment.Position;
    payload.m_distance = RayTCurrent();
}