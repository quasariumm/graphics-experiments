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