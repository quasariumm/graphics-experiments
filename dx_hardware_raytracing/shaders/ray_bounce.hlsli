#ifndef _RAY_BOUNCE_HLSLI
#define _RAY_BOUNCE_HLSLI

#include "materials.hlsli"

uint Randomise(in uint seed)
{
	seed = seed * 747796405u + 2891336453u;
	seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
	seed = (seed >> 22u) ^ seed;
	return seed;
}

float RandomFloat(inout uint seed)
{
	return Randomise(seed) / 4294967295.f;
}

float3 SampleGGX(inout uint seed, float3 normal, float3 tangent, float3 bitangent, float3 wo, float roughness)
{
	// https://jcgt.org/published/0007/04/01/paper.pdf
	float alpha = roughness * roughness;

	// Transform view direction to local space
	float3 wo_local = float3(dot(wo, tangent), dot(wo, bitangent), dot(wo, normal));

	// Section 3.2: transforming the view direction to the hemisphere configuration
	float3 wh = normalize(float3(alpha * wo_local.x, alpha * wo_local.y, wo_local.z));

	// Section 4.1: orthonormal basis
	float lensq = wh.x * wh.x + wh.y * wh.y;
	float3 T1 = lensq > 0.0 ? float3(-wh.y, wh.x, 0.0) / sqrt(lensq) : float3(1.0, 0.0, 0.0);
	float3 T2 = cross(wh, T1);

	// Section 4.2: parameterization of the projected area
	float r1 = RandomFloat(seed);
	float r2 = RandomFloat(seed);
	float r = sqrt(r1);
	float phi = 2.0 * pi * r2;
	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5 * (1.0 + wh.z);
	t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

	// Section 4.3: reprojection onto hemisphere
	float3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * wh;

	// Section 3.4: transforming the normal back to the ellipsoid configuration
	float3 Ne = normalize(float3(alpha * Nh.x, alpha * Nh.y, max(0.0, Nh.z)));

	// Transform back to world space
	return normalize(Ne.x * tangent + Ne.y * bitangent + Ne.z * normal);
}

uint RaySeed(in HitInfo hitInfo, in uint frameNum)
{
	uint3 idx = DispatchRaysIndex();
	idx += 1u;
	uint seed = idx.x ^ Randomise(idx.y << 3);
	seed ^= Randomise(GetRecursionDepth(hitInfo.m_flags) + frameNum);
	return Randomise(seed);
}

RayDesc ShadowRay(in ShaderLight light, in float3 hitPos, in float3 wi)
{
	RayDesc desc;
	desc.Origin = hitPos;
    desc.TMin = 0.001;
	if (light.m_type == light_type_punctual)
		desc.TMax = length(light.m_position - hitPos) - 0.001;
	else
		desc.TMax = 10000.0;
	desc.Direction = wi;

	return desc; 
}

RayDesc BounceRay(
	in HitInfo hitInfo, in float3 hitPos, 
	in float3 wo, in FragmentAttributes attributes, 
	in uint frameNum
)
{
    RayDesc desc;
    desc.Origin = hitPos + 0.001 * attributes.m_normal;
    desc.TMin = 0.001;
    desc.TMax = 10000.0;

    if (attributes.m_roughness < 0.0001)
    {
        desc.Direction = reflect(wo, attributes.m_normal);
    }
    else
    {
        uint seed = RaySeed(hitInfo, frameNum);
        desc.Direction = SampleGGX(
			seed, 
			attributes.m_normal, 
			attributes.m_tangent, 
			attributes.m_bitangent, 
			wo, 
			attributes.m_roughness
		);
    }

    return desc;
}

#endif