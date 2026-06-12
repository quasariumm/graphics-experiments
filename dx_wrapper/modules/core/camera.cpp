module;

module dx_wrapper.core.camera;
import dx_wrapper.core.input;

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

void Camera::HandleInput(DxDevice& device)
{
	Input&	   input		   = device.GetInput();
	Transform& cameraTransform = GetTransform();

	static constexpr float camera_speed		 = 5.0f;
	static constexpr float mouse_sensitivity = 0.1f;

	static float yaw   = 0.0f;
	static float pitch = 0.0f;

	const float deltaTime = device.GetDeltaTime();

	if (input.GetKeyboardKey(Key::W))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) +
									deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::S))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) -
									deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::A))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) -
									deltaTime * camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::D))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) +
									deltaTime * camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::E))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) +
									deltaTime * camera_speed * cameraTransform.GetUp(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::Q))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) -
									deltaTime * camera_speed * cameraTransform.GetUp(TransformSpace::Local));

	if (input.GetMouseButton(MouseButton::Right))
	{
		glm::vec2 delta = input.GetMouseDelta();

		yaw -= delta.x * mouse_sensitivity;
		pitch -= delta.y * mouse_sensitivity;
		pitch = glm::clamp(pitch, -89.0f, 89.0f);

		glm::quat qYaw	 = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

		cameraTransform.SetRotation(glm::normalize(qYaw * qPitch));
	}
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
