#include "shader_resources.hlsli"
#include "meshlets.hlsli"

StructuredBuffer<Meshlet> Meshlets          : register(t0);
Buffer<uint>              MeshletVertices   : register(t1);
ByteAddressBuffer         MeshletTriangles  : register(t2);

StructuredBuffer<Vertex>  Vertices : register(t3);

ConstantBuffer<CameraConstBuffer> CameraCB     : register(b0);
ConstantBuffer<ShaderTransform>   Transform32C : register(b2);

uint3 LoadMeshletTriangle(uint baseByteOffset, uint triIndex)
{
    uint byteOffset  = baseByteOffset + triIndex * 3;
    uint wordOffset  = byteOffset & ~3u;
    uint byteShift   = byteOffset & 3u;

    uint word0 = MeshletTriangles.Load(wordOffset);
    // Spills into next word when byteShift >= 2 (only 2 or 1 bytes left in word0)
    uint word1 = (byteShift >= 2u) ? MeshletTriangles.Load(wordOffset + 4) : 0u;

    uint data = word0 >> (byteShift * 8);
    data     |= word1 << ((4u - byteShift) * 8);

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

    Meshlet m = Meshlets[meshletIndex];
    SetMeshOutputCounts(m.VertexCount, m.TriangleCount);

    if (gtId < m.TriangleCount) {
        triangles[gtId] = LoadMeshletTriangle(m.TriangleOffset, gtId);
    }

    if (gtId < m.VertexCount) {
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
        
        float3 color = float3(
            float(meshletIndex & 1),
            float(meshletIndex & 3) / 4,
            float(meshletIndex & 7) / 8);
        vertices[gtId].Color = color;
    }
}