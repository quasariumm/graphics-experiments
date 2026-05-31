#ifndef _LIGHTING_HLSLI
#define _LIGHTING_HLSLI

#include "common.hlsli"

ShaderLight CreateDirectionalLight(in float3 direction, in float3 color, in float intensity)
{
    ShaderLight light = (ShaderLight)0;
    light.m_type = light_type_directional;
    light.m_intensity = intensity;
    light.m_direction = direction;
    light.m_color = direction;
    return light;
}

float3 SampleLight(ShaderLight light, float3 hitPos, float3 normal)
{
    switch (light.m_type)
    {
    case light_type_punctual:
    {
        float3 lightDir = hitPos - light.m_position;
        float lightDist = length(lightDir);
        lightDir /= lightDist;
        
        // GLTF 2.0 uses quadratic attenuation, but with range
        float x = lightDist / light.m_range;
        float attenuation = saturate(1.0 - x * x * x * x) / (lightDist * lightDist);
        
        // Spotlight cone
        if (light.m_outerAngle < pi)
        {
            float cd = dot(-light.m_direction, -lightDir);
            float cosOuter = cos(light.m_outerAngle);
            if (cd < cosOuter)
                return 0.0;

            float spotScale = 1.0 / max(cos(light.m_innerAngle) - cosOuter, 1e-4f);
            float spotOffset = -cosOuter * spotScale;
            float spotAtt = saturate(cd * spotScale + spotOffset);
            attenuation *= spotAtt * spotAtt;
        }

        float NdotL = dot(normal, -light.m_direction);

        return light.m_color * light.m_intensity * NdotL * attenuation;
    }
    case light_type_directional:
    {
        float NdotL = dot(-light.m_direction, normal);
        if (NdotL < 0.0)
            return 0.0;
        return light.m_color * light.m_intensity * NdotL;
    }
    default:
        return 0.0;
    }
}

#endif