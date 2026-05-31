#ifndef _PBR_HLSLI
#define _PBR_HLSLI

#include "common.hlsli"

// Implemented PBR code with the help of: https://youtu.be/gya7x9H3mV0?si=vo547-B1ZZF2oxy4
// and https://typhomnt.github.io/teaching/ray_tracing/pbr_intro/
// and https://learnopengl.com/PBR/Lighting

float3 FresnelSchlick(float HdotO, float3 f0)
{
    float x = 1.0 - HdotO;
    return f0 + (1.0 - f0) * x * x * x * x * x;
}
float3 FresnelSchlickRoughness(float HdotO, float3 f0, float roughness)
{
    float x = 1.0 - HdotO;
    return f0 + (max((1.0 - roughness), f0) - f0) * x * x * x * x * x;
}

float GGXIsoD(float alpha, float NdotH)
{
    float alphaSquared = alpha * alpha;
    
    float val = NdotH * NdotH * (alphaSquared - 1.0) + 1.0;
    
    return alphaSquared / (pi * val * val);
}

float GGXIsoG1(float k, float NdotW)
{
    return NdotW / (NdotW * (1.0 - k) + k);
}

float GGXIsoG(float alpha, float NdotO, float NdotI)
{
    float k = alpha / 2.0;
    
    return GGXIsoG1(k, NdotO) * GGXIsoG1(k, NdotI);
}

float GGXIsoV(float alpha, float NdotO, float NdotI)
{
    return lerp(2.0 * NdotO * NdotI, NdotO + NdotI, alpha * alpha);
}

float3 ComputeBRDF(float3 normal, float3 wo, float3 wi, float3 baseColor, float roughness, float metallic)
{
    float3 H = normalize(wo + wi);
    
    float NdotO = max(dot(normal, wo), 0.0001);
    float NdotI = max(dot(normal, wi), 0.0001);
    float NdotH = max(dot(normal, H), 0.0001);
    float HdotO = max(dot(H, wo), 0.0001);
    
    float3 f0 = lerp(0.04, baseColor, metallic);
    
    float alpha = max(roughness * roughness, 0.001); // Minimum alpha value

    float3 fresnel = FresnelSchlick(HdotO, f0);
    float distribution = GGXIsoD(alpha, NdotH);
    float visibility = GGXIsoV(alpha, NdotI, NdotO);
    
    float3 spec = (fresnel * distribution * visibility);
    
    float3 diff = baseColor * (1.0 - metallic) * (1.0 - fresnel) * inv_pi;
    
    return diff + spec;
}

float ComputePDF(float3 normal, float3 wo, float3 wi, float roughness)
{
	if (dot(normal, wi) <= 0.0 || dot(normal, wo) <= 0.0) return 0.0;

	const float3 wh = normalize(wo + wi);
	float alpha = max(0.001, roughness * roughness);

	float NdotO = clamp(dot(normal, wo), 0.0001, 1.0);
	float NdotH = saturate(dot(normal, wh));

    float D = GGXIsoD(alpha, NdotH);
    float G1 = GGXIsoG1(alpha, NdotO);

	return G1 * D / (4.0 * NdotO);
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 random, float alpha2)
{
    float phi = 2.0 * pi * random.x;
    float cosTheta = sqrt((1.0 - random.y) / (1.0 + (alpha2 - 1.0) * random.y));
    
    // For some reason this created precision issues
    // TODO: Figure out why the precision issues are happening
    //float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float sinTheta = sin(acos(cosTheta));
	
    float3 localSample = float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	
    return localSample;
}

#endif