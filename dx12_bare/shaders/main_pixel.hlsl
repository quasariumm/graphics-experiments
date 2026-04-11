#include "shader_resources.hlsli"
#include "pbr.hlsli"

struct PSIn
{
    float2 Uv0 : TEXCOORD0;
    float2 Uv1 : TEXCOORD1;
    float3 WorldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 Position : SV_Position;
};

ConstantBuffer<CameraConstBuffer> CameraCB : register(b0);
ConstantBuffer<ShaderMaterial> MaterialC : register(b1);

SamplerState SSLinearWrap : register(s0);


float3 CalculateDirectionalLight(in ShaderLight light,
                                 in float3 wo,
                                 in float3 normal,
                                 in float3 albedo,
                                 in float roughness,
                                 in float metallic)
{
    const float3 lightColor = light.m_color * light.m_intensity;

    const float3 lightDir = -normalize(light.m_direction);

    float NdotL = dot(normal, lightDir);
    if (NdotL > 0.0)
    {
        float3 brdf = ComputeBRDF(normal, -wo, lightDir, albedo.rgb, roughness, metallic);
        
        return brdf * NdotL * lightColor;
    }
    return 0.0;
}


float4 main(PSIn IN) : SV_Target
{
    // Variables
    float4 albedo = MaterialC.m_baseColorFactor;
    float roughness = MaterialC.m_roughnessFactor;
    float metallic = MaterialC.m_metallicFactor;
    float3 normal = IN.Normal;
    float3 emissive = MaterialC.m_emissiveFactor;
    float occlusion = MaterialC.m_occlusionStrength;

    // Base Color texture
    if (MaterialC.m_texIndices1[TEX_ALBEDO] != -1)
    {
        Texture2D<float4> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_ALBEDO]];
        albedo *= tex.Sample(SSLinearWrap, IN.Uv0);
    }

    // Alpha Mode: Mask
    if ((MaterialC.m_flags & 0x3) == 0x1 && albedo.a < MaterialC.m_alphaCutoff)
        discard;

    // PBR Texture
    if (MaterialC.m_texIndices2[TEX_ROUGH_METAL] != -1)
    {
        Texture2D<float4> tex = ResourceDescriptorHeap[MaterialC.m_texIndices2[TEX_ROUGH_METAL]];
        float2 texSample = tex.Sample(SSLinearWrap, IN.Uv0).gb;
        roughness *= texSample.x;
        metallic *= texSample.y;
    }

    // Normal mapping
    if (MaterialC.m_texIndices1[TEX_NORMAL] != -1)
    {
        Texture2D<float3> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_NORMAL]];
        
        float3 vNormal = normalize(IN.Normal);
        float3 vTangent = normalize(IN.Tangent.xyz);
        float3 vBitangent = cross(vNormal, vTangent) * IN.Tangent.w;
    
        float3x3 TBN = transpose(float3x3(vTangent, vBitangent, vNormal));
    
        normal = tex.Sample(SSLinearWrap, IN.Uv0).rgb;
        normal = normal * 2.0f - 1.0f;
        normal *= float3(MaterialC.m_normalScale, MaterialC.m_normalScale, 1.0f);
        normal = normalize(mul(TBN, normal));
    }

    // Emissive
    if (MaterialC.m_texIndices1[TEX_EMISSIVE] != -1)
    {
        Texture2D<float3> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_EMISSIVE]];
        emissive *= tex.Sample(SSLinearWrap, IN.Uv0).rgb;
    }

    // Occlusion
    if (MaterialC.m_texIndices1[TEX_OCCLUSION] != -1)
    {
        Texture2D<float> tex = ResourceDescriptorHeap[MaterialC.m_texIndices1[TEX_OCCLUSION]];
        occlusion *= tex.Sample(SSLinearWrap, IN.Uv0);
    }

    // Lighting
    float3 wo = normalize(IN.WorldPos - CameraCB.m_shaderCamera.m_cameraPosition);

    float3 radiance = emissive;

    ShaderLight keyLight;
    keyLight.m_position    = float3(0.0, 0.0, 0.0);
    keyLight.m_type        = 1;
    keyLight.m_direction   = normalize(float3(-1.0, -1.0, 0.5));
    keyLight.m_range       = 0.0;
    keyLight.m_color       = float3(1.0, 1.0, 1.0);
    keyLight.m_intensity   = 1.0;
    keyLight.m_innerAngle  = 0.0;
    keyLight.m_outerAngle  = 0.0;
    keyLight.m_castShadows = 0;
    keyLight.m_padding     = 0.0;

    ShaderLight fillLight;
    fillLight.m_position    = float3(0.0, 0.0, 0.0);
    fillLight.m_type        = 1;
    fillLight.m_direction   = normalize(float3(0.5, -0.5, -1.0));
    fillLight.m_range       = 0.0;
    fillLight.m_color       = float3(0.7, 0.7, 0.8); // slightly cool tint
    fillLight.m_intensity   = 0.5;
    fillLight.m_innerAngle  = 0.0;
    fillLight.m_outerAngle  = 0.0;
    fillLight.m_castShadows = 0;
    fillLight.m_padding     = 0.0;

    radiance += CalculateDirectionalLight(keyLight, wo, normal, albedo.rgb, roughness, metallic);
    radiance += CalculateDirectionalLight(fillLight, wo, normal, albedo.rgb, roughness, metallic);
    
    return float4(max(radiance * occlusion, 0.0), 1.0);
}