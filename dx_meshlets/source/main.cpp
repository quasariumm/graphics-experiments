import dx_wrapper.core;
import dx_meshlets.gltf.model;
import dx_meshlets.rendering.dx_renderer;
import dx_meshlets.core.camera;

struct Demo
{
	DxDevice m_device{1920, 1080, "DX12 Meshlets", D3D_FEATURE_LEVEL_12_2};
	DxRenderer m_renderer{&m_device};
	
	void Run();
	void HandleInput();
};

int main()
{
	Demo demo{};
	demo.Run();
	return 0;
}

void Demo::Run()
{
	m_renderer.AddModel("../dx_bare/assets/DamagedHelmet/DamagedHelmet.gltf");
	
	while (!m_device.ShouldClose())
	{
		m_device.BeginFrame();
		HandleInput();
		m_renderer.Render();
		m_device.EndFrame();
	}
}

void Demo::HandleInput()
{
	Input& input = m_device.GetInput();
	Camera& camera = m_renderer.GetCamera();
	Transform& cameraTransform = camera.GetTransform();
	
	static constexpr float camera_speed = 5.0f;
	
	float deltaTime = m_device.GetDeltaTime();
	
	if (input.GetKeyboardKey(Key::W))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::S))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::A))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + deltaTime * camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::D))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - deltaTime * camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::E))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + deltaTime * camera_speed * cameraTransform.GetUp(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::Q))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - deltaTime * camera_speed * cameraTransform.GetUp(TransformSpace::Local));
	
	if (input.GetMouseButton(MouseButton::Right))
	{
		glm::vec2 delta = input.GetMouseDelta();

		static constexpr float mouse_sensitivity = 0.5f;

		glm::quat yaw   = glm::angleAxis(delta.x * deltaTime * mouse_sensitivity, glm::vec3(0, 1, 0));
		glm::quat pitch = glm::angleAxis(-delta.y * deltaTime * mouse_sensitivity, cameraTransform.GetRight(TransformSpace::Local));

		cameraTransform.SetRotation(glm::normalize(pitch * yaw * cameraTransform.GetRotation()));
	}
}
