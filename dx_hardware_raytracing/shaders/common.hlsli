#ifndef _COMMON_HLSLI
#define _COMMON_HLSLI

static const float pi = 3.141592653589;
static const float inv_pi = 0.31830988618;

uint GetRecursionDepth(in uint flags) { return flags & 0x1fu; }
void IncRecursionDepth(inout uint flags) { flags += 1u; }

struct [raypayload] HitInfo
{
    float3 m_color    : read(caller, closesthit)       : write(caller, closesthit, miss);
    float m_distance  : read(caller, closesthit)       : write(caller, closesthit, miss);
    /** 
        recursionDepth (5 bits)
    */
    uint m_flags      : read(caller, closesthit, miss) : write(caller, closesthit, miss);
    float3 m_rayColor : read(caller, closesthit, miss) : write(caller, closesthit, miss);
};

struct [raypayload] ShadowPayload 
{
    uint m_occluded : read(caller) : write(caller, miss);
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

static const uint light_type_punctual = 0;
static const uint light_type_directional = 1;

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
    uint2 m_padding;
};

static const uint tex_albedo      = 0;
static const uint tex_normal      = 1;
static const uint tex_emissive    = 2;
static const uint tex_occlusion   = 3;
static const uint tex_rough_metal = 4;

struct ShaderMaterial 
{
    float m_alphaCutoff;

	float3 m_emissiveFactor;
	float4 m_baseColorFactor;

	float m_metallicFactor;
	float m_roughnessFactor;
	float m_normalScale;
	float m_occlusionStrength;

    int m_albedoTex;
    int m_normalTex;
    int m_emissiveTex;
    int m_occlusionTex;
    int m_roughMetalTex;
    int m_tex6;
    int m_tex7;
    int m_tex8;
	
    uint m_flags;
    uint3 m_padding;
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

/*
    Debug modes
*/
static const uint debug_none             = 0;
static const uint debug_albedo           = 1;
static const uint debug_alpha            = 2;
static const uint debug_emissive         = 3;
static const uint debug_roughness        = 4;
static const uint debug_metallic         = 5;
static const uint debug_transmissive     = 6;
static const uint debug_ior              = 7;
static const uint debug_geom_normal      = 8;
static const uint debug_shaded_normal    = 9;
static const uint debug_geom_tangent     = 10;
static const uint debug_shaded_tangent   = 11;
static const uint debug_geom_bitangent   = 12;
static const uint debug_shaded_bitangent = 13;
static const uint debug_brdf             = 14;
static const uint debug_pdf              = 15;

#endif