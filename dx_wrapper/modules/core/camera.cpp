module;

module dx_wrapper.core.camera;

Camera::Camera(const DxDevice* device)
{
	m_shaderCamera.m_viewProjectionMatrix	  = glm::mat4(1.0f);
	m_shaderCamera.m_prevViewProjectionMatrix = glm::mat4(1.0f);
	m_shaderCamera.m_cameraPosition			  = glm::vec3(0.0f);

	m_shaderCamera.m_nearPlane = m_nearPlane;
	m_shaderCamera.m_farPlane  = m_farPlane;

	m_shaderCamera.m_prevCameraPosition = glm::vec3(0.0f);

	m_aspect = device ? static_cast<float>(device->GetWidth()) / static_cast<float>(device->GetHeight()) : 16.f / 9.f;
	UpdateProjectionMatrix(m_aspect);
}

void Camera::SetFov(const float fov)
{
	m_fov = fov;
	UpdateProjectionMatrix(m_aspect);
}

void Camera::SetNearPlane(const float nearPlane)
{
	m_nearPlane				   = nearPlane;
	m_shaderCamera.m_nearPlane = nearPlane;
	UpdateProjectionMatrix(m_aspect);
}

void Camera::SetFarPlane(const float farPlane)
{
	m_farPlane				  = farPlane;
	m_shaderCamera.m_farPlane = farPlane;
	UpdateProjectionMatrix(m_aspect);
}

void Camera::SetAspectRatio(const float aspect)
{
	UpdateProjectionMatrix(aspect);
}

void Camera::SetAspectRatio(const DxDevice& device)
{
	UpdateProjectionMatrix(static_cast<float>(device.GetWidth()) / static_cast<float>(device.GetHeight()));
}

void Camera::UpdateShaderCamera()
{
	// Implemented this to update the projection matrix of a newly serialized camera component
	if (m_projMatrixDirty == true)
	{
		UpdateProjectionMatrix(m_aspect);
		m_projMatrixDirty = false;
	}

	m_shaderCamera.m_prevViewProjectionMatrix = m_shaderCamera.m_viewProjectionMatrix;
	m_shaderCamera.m_prevCameraPosition		  = m_shaderCamera.m_cameraPosition;

	glm::vec3 pos	  = m_transform.GetPosition();
	glm::vec3 forward = m_transform.GetForward();
	glm::vec3 up	  = m_transform.GetUp();

	m_viewMatrix = glm::lookAt(pos, pos + forward, up);

	m_shaderCamera.m_viewMatrix			  = m_viewMatrix;
	m_shaderCamera.m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	m_shaderCamera.m_cameraPosition		  = pos;
}

void Camera::UpdateProjectionMatrix(const float aspect)
{
	m_aspect		   = aspect;
	m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_nearPlane, m_farPlane);
}
