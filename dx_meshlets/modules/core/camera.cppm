module;

export module dx_meshlets.core.camera;
import dx_wrapper.core;
import dx_wrapper.external.glm;

export class Camera
{
public:

	/**
	 * @brief Constructs a new camera
	 * @param device [optional, default = nullptr] The device. Used for getting the aspect ratio. When @c nullptr, it defaults to 16/9
	 */
	explicit Camera(const DxDevice* device = nullptr);
	
	Transform& GetTransform() { return m_transform; }
	const Transform& GetTransform() const { return m_transform; }
	
	void SetFov(const float fov)
	{
		m_fov = fov;
		UpdateProjectionMatrix(m_aspect);
	}
	void SetNearPlane(const float nearPlane)
	{
		m_nearPlane = nearPlane;
		m_shaderCamera.m_nearPlane = nearPlane;
		UpdateProjectionMatrix(m_aspect);
	}
	void SetFarPlane(const float farPlane)
	{
		m_farPlane = farPlane;
		m_shaderCamera.m_farPlane = farPlane;
		UpdateProjectionMatrix(m_aspect);
	}

	float GetFov() const { return m_fov; }
	float GetNearPlane() const { return m_nearPlane; }
	float GetFarPlane() const { return m_farPlane; }
	float GetAspectRatio() const { return m_aspect; }

	glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	
	/*
	 * Shader camera
	 */
	
	struct ShaderCamera
	{
		glm::mat4 m_viewProjectionMatrix;
		glm::mat4 m_prevViewProjectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::vec3 m_cameraPosition;

		float m_nearPlane;
		float m_farPlane;

		glm::vec3 m_prevCameraPosition;
	};

	// Call this before using ShaderCamera in shaders
	void UpdateShaderCamera();
	const ShaderCamera& GetShaderCamera() const { return m_shaderCamera; }
	
private:
	
	Transform m_transform;
	
	ShaderCamera m_shaderCamera = {};

	float m_fov = 70.0f;
	float m_nearPlane = 0.1f;
	float m_farPlane = 1000.0f;
	float m_aspect = 1.0f;

	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

	bool m_projMatrixDirty = true;

	// This function should also be called when there is a change in the viewport size
	void UpdateProjectionMatrix(float aspect);
};