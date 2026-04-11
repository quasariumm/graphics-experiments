#define TEX_ALBEDO      0
#define TEX_NORMAL      1
#define TEX_EMISSIVE    2
#define TEX_OCCLUSION   3
// m_texIndices2
#define TEX_ROUGH_METAL 0

struct ShaderTransform
{
    float4x4 m_worldMatrix;
    float4x4 m_normalMatrix;
};

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
    uint3 m_padding;
};

struct ShaderCamera
{
    float4x4 m_viewProjectionMatrix;
    float4x4 m_prevViewProjectionMatrix;
    float4x4 m_viewMatrix;
    float3 m_cameraPosition;

    float m_nearPlane;
    float m_farPlane;

    float3 m_prevCameraPosition;
};

struct CameraConstBuffer
{
    ShaderCamera m_shaderCamera;

    float4 m_padding[2];
};