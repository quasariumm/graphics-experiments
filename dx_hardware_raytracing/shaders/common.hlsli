struct [raypayload] HitInfo
{
    float3 m_color : read(caller) : write(caller, closesthit, miss);
    float m_distance : read(caller) : write(caller, closesthit, miss);
};

struct Attributes
{
    float2 m_bary;
};

struct ShaderCamera
{
    float4x4 m_viewProjection;
    float4x4 m_inverseViewProjection;
    float4x4 m_view;
    float3 m_cameraPosition;

    float m_nearPlane;
    float m_farPlane;

    float3 m_prevCameraPosition;
};

#define TEX_ALBEDO      0
#define TEX_NORMAL      1
#define TEX_EMISSIVE    2
#define TEX_OCCLUSION   3
// m_texIndices2
#define TEX_ROUGH_METAL 0

struct ShaderMaterial 
{
    float m_alphaCutoff;

	float3 m_emissiveFactor;
	float4 m_baseColorFactor;

	float m_metallicFactor;
	float m_roughnessFactor;
	float m_normalScale;
	float m_occlusionStrength;

    int4 m_texIndices1;
    int4 m_texIndices2;
	
    uint m_flags;
    uint m_debugMode;
    uint2 m_padding;
};

struct Vertex
{
    float3 Position;
    float _padding1;
    float2 Uv0;
    float2 Uv1;
    float3 Normal;
    float _padding2;
    float4 Tangent;
};