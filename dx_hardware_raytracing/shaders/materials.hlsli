#ifndef _MATERIALS_HLSLI
#define _MATERIALS_HLSLI

#include "common.hlsli"

struct FragmentAttributes
{
    // Colour data
    float4 m_albedo;
    float3 m_emissive;
    
    // PBR data
    float m_roughness;
    
    // Geometry data
    float3 m_normal;

    float m_metallic; // Here due to better alignment

    float3 m_tangent;

    float m_transmission; // Implemented later on

    float3 m_bitangent;

    float m_ior; // Implemented later on
};

float  SampleTexture(in Texture2D<float> tex,  in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index);
float2 SampleTexture(in Texture2D<float2> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index);
float3 SampleTexture(in Texture2D<float3> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index);
float4 SampleTexture(in Texture2D<float4> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index);

FragmentAttributes GetFragmentAttributes(in SamplerState samplerState, in Vertex fragment, in ShaderMaterial material) 
{
    FragmentAttributes attributes = (FragmentAttributes)0;
    
    // TODO: Add textures or params for this
    attributes.m_transmission = 0.0;
    attributes.m_ior = 1.5;

    // Albedo
    if (material.m_albedoTex != -1)
    {
        Texture2D<float4> albedoTex = ResourceDescriptorHeap[material.m_albedoTex];
        attributes.m_albedo = SampleTexture(albedoTex, samplerState, fragment.Uv0, fragment.Uv1, material.m_flags, tex_albedo);
        attributes.m_albedo *= material.m_baseColorFactor;
    }

    // Emissive
    if (material.m_emissiveTex != -1)
    {
        Texture2D<float3> emissiveTex = ResourceDescriptorHeap[material.m_emissiveTex];
        attributes.m_emissive = SampleTexture(emissiveTex, samplerState, fragment.Uv0, fragment.Uv1, material.m_flags, tex_emissive);
        attributes.m_emissive *= material.m_emissiveFactor;
    }

    // Roughness + Metallic
    if (material.m_roughMetalTex != -1)
    {
        Texture2D<float3> ormTex = ResourceDescriptorHeap[material.m_roughMetalTex];
        float2 roughMetal = SampleTexture(ormTex, samplerState, fragment.Uv0, fragment.Uv1, material.m_flags, tex_rough_metal).gb;
        attributes.m_roughness = roughMetal.r * material.m_roughnessFactor;
        attributes.m_metallic = roughMetal.g * material.m_metallicFactor;
    }

    // Normal, Tangent, Bitangent
    if (material.m_normalTex != -1)
    {
        Texture2D<float3> normalTex = ResourceDescriptorHeap[material.m_normalTex];
        float3 normalSample = SampleTexture(normalTex, samplerState, fragment.Uv0, fragment.Uv1, material.m_flags, tex_normal);
        normalSample = 2.0 * normalSample - 1.0;
        normalSample *= float3(material.m_normalScale, material.m_normalScale, 1.0);

        float3x3 TBN = float3x3(fragment.Tangent.xyz, fragment.Tangent.w * cross(fragment.Normal, fragment.Tangent.xyz), fragment.Normal);

        // Same as mul(transpose(TBN), normalSample)
        attributes.m_normal = mul(normalSample, TBN);
        attributes.m_tangent = normalize(fragment.Tangent.xyz - dot(fragment.Tangent.xyz, attributes.m_normal) * attributes.m_normal);
        attributes.m_bitangent = fragment.Tangent.w * cross(attributes.m_normal, attributes.m_tangent);
    }
    else 
    {
        attributes.m_normal = fragment.Normal;
        attributes.m_tangent = fragment.Tangent.xyz;
        attributes.m_tangent = fragment.Tangent.w * cross(fragment.Normal, fragment.Tangent.xyz);
    }

    // Alpha masking
    if ((material.m_flags & 0b111) == 1 && attributes.m_albedo.a < material.m_alphaCutoff)
        attributes.m_albedo.a = 0.0;

    return attributes;
}


float3 GetDebugOutput(in Vertex geometryData, in FragmentAttributes attributes, in uint debugMode)
{
    switch (debugMode)
    {
    case debug_albedo:
        return attributes.m_albedo.rgb;
    case debug_alpha:
        return attributes.m_albedo.a;
    case debug_emissive:
        return attributes.m_emissive;
    case debug_roughness:
        return attributes.m_roughness;
    case debug_metallic:
        return attributes.m_metallic;
    case debug_transmissive:
        return attributes.m_transmission;
    case debug_ior:
        return attributes.m_ior;
    case debug_geom_normal:
        return 0.5 * geometryData.Normal + 0.5;
    case debug_shaded_normal:
        return 0.5 * attributes.m_normal + 0.5;
    case debug_geom_tangent:
        return 0.5 * geometryData.Tangent.xyz + 0.5;
    case debug_shaded_tangent:
        return 0.5 * attributes.m_tangent + 0.5;
    case debug_geom_bitangent:
        return 0.5 * geometryData.Tangent.w * cross(geometryData.Normal, geometryData.Tangent.xyz) + 0.5;
    case debug_shaded_bitangent:
        return 0.5 * attributes.m_bitangent + 0.5;
    default:
        return 0.0;
    }
}


float SampleTexture(in Texture2D<float> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index)
{
    float2 uv = ((flags & (1u << (16u + index))) != 0u) ? uv1 : uv0;
    return tex.SampleLevel(samplerState, uv, 0.0);
}

float2 SampleTexture(in Texture2D<float2> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index)
{
    float2 uv = ((flags & (1u << (16u + index))) != 0u) ? uv1 : uv0;
    return tex.SampleLevel(samplerState, uv, 0.0);
}

float3 SampleTexture(in Texture2D<float3> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index)
{
    float2 uv = ((flags & (1u << (16u + index))) != 0u) ? uv1 : uv0;
    return tex.SampleLevel(samplerState, uv, 0.0);
}

float4 SampleTexture(in Texture2D<float4> tex, in SamplerState samplerState, in float2 uv0, in float2 uv1, in uint flags, in uint index)
{
    float2 uv = ((flags & (1u << (16u + index))) != 0u) ? uv1 : uv0;
    return tex.SampleLevel(samplerState, uv, 0.0);
}

#endif