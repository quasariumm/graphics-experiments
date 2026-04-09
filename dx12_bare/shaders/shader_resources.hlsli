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