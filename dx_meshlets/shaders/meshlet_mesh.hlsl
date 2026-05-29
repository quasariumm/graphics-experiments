#include "shader_resources.hlsli"
#include "meshlets.hlsli"

StructuredBuffer<Meshlet> Meshlets          : register(t0);
Buffer<uint>              MeshletVertices   : register(t1);
Buffer<uint>              MeshletTriangles  : register(t2);

StructuredBuffer<Vertex>  Vertices : register(t3);

ConstantBuffer<CameraConstBuffer> CameraCB     : register(b0);
ConstantBuffer<ShaderTransform>   Transform32C : register(b2);

uint3 LoadMeshletTriangle(uint baseByteOffset, uint triIndex)
{
    uint data = MeshletTriangles.Load(baseByteOffset + triIndex);

    return uint3(data & 0xFF, (data >> 8) & 0xFF, (data >> 16) & 0xFF);
}

[outputtopology("triangle")]
[numthreads(128, 1, 1)]
void main(uint gtId : SV_GroupThreadID, 
          uint gId  : SV_GroupID, 
          in payload Payload payload,
          out indices uint3 triangles[128], 
          out vertices MSOut vertices[64]) 
{
    uint meshletIndex = payload.MeshletIndices[gId];

    uint meshletCount, stride;
    Meshlets.GetDimensions(meshletCount, stride);

    uint safeIndex = min(meshletIndex, meshletCount - 1);
    Meshlet m = Meshlets[safeIndex];

    bool valid = meshletIndex < meshletCount;
    SetMeshOutputCounts(valid ? m.VertexCount : 0, valid ? m.TriangleCount : 0);

    if (!valid || m.TriangleCount == 0)
        return;

    if (m.TriangleCount == 0)
        return;

    if (gtId < m.TriangleCount)
        triangles[gtId] = LoadMeshletTriangle(m.TriangleOffset, gtId);

    if (gtId < m.VertexCount) 
    {
        uint vertexIndex = m.VertexOffset + gtId;        
        vertexIndex = MeshletVertices[vertexIndex];

        float4x4 mvp = mul(CameraCB.m_shaderCamera.m_viewProjectionMatrix, Transform32C.m_worldMatrix);

        Vertex vertex = Vertices[vertexIndex];
        vertices[gtId].WorldPos = mul(Transform32C.m_worldMatrix, float4(vertex.Position, 1.0)).xyz;
        vertices[gtId].Position = mul(mvp, float4(vertex.Position, 1.0));
        vertices[gtId].Uv0 = vertex.Uv0;
        vertices[gtId].Uv1 = vertex.Uv1;
        vertices[gtId].Normal = mul(Transform32C.m_normalMatrix, float4(vertex.Normal, 0.0)).xyz;
        vertices[gtId].Tangent = float4(mul(Transform32C.m_normalMatrix, float4(vertex.Tangent.xyz, 0.0)).xyz, vertex.Tangent.w);
        vertices[gtId].Color = 1.0;
        vertices[gtId].MeshletIndex = meshletIndex;
    }
}