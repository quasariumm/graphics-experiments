struct Meshlet
{
	uint VertexOffset;
	uint TriangleOffset;
	uint VertexCount;
	uint TriangleCount;
};

/*
    Amplification
*/
static const uint AS_GROUP_SIZE = 32;

struct Payload 
{
    uint MeshletIndices[AS_GROUP_SIZE];
};

/*
    Mesh
*/
struct Vertex
{
    float3 Position;
    float2 Uv0;
    float2 Uv1;
    float3 Normal;
    float4 Tangent;
};

struct MSOut
{
    float2 Uv0 : TEXCOORD0;
    float2 Uv1 : TEXCOORD1;
    float3 WorldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 Position : SV_Position;
    float3 Color : COLOR0;
};