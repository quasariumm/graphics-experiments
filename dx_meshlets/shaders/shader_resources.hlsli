#define TEX_ALBEDO      0
#define TEX_NORMAL      1
#define TEX_EMISSIVE    2
#define TEX_OCCLUSION   3
// m_texIndices2
#define TEX_ROUGH_METAL 0

struct ShaderLight
{
    float3 m_position;
    int m_type;
    float3 m_direction;
    float m_range;
    float3 m_color;
    float m_intensity;
    float m_innerAngle;
    float m_outerAngle;
    uint m_castShadows;

    float m_padding;
};

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
    uint m_debugMode;
    uint2 m_padding;
};

struct ShaderCamera
{
    column_major float4x4 m_viewProjectionMatrix;
    column_major float4x4 m_prevViewProjectionMatrix;
    column_major float4x4 m_viewMatrix;
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