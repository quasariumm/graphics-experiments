import dx_wrapper.core;
import std;
import dx_hw_ray.rendering.dx_render;
import dx_hw_ray.rendering.imgui_renderer;
import dx_hw_ray.external.imgui;

struct Demo
{
	explicit Demo(const DxDevice::DebugLayerMode debugLayerMode = DxDevice::DebugLayerMode::Enabled)
		: m_device{1920, 1080, "DXR Hardware Raytracing", D3D_FEATURE_LEVEL_12_2, debugLayerMode}, m_renderer{&m_device},
		  m_imguiRenderer{&m_device}
	{}

	DxDevice	  m_device;
	DxRenderer	  m_renderer;
	ImGuiRenderer m_imguiRenderer;

	void Run();
	void HandleInput();
	void DrawImGuiWindow();
};

int main(const int argc, char** argv)
{
	Demo demo{DxDevice::GetDebugLayerModeFromArgs(argc, argv)};
	demo.Run();
	return 0;
}

void Demo::Run()
{
	if (!m_device.SupportsRaytracing())
		Log::Critical("You need raytracing support to run this demo");

	m_renderer.AddModel("../dx_bare/assets/Sponza/Sponza.gltf");

	while (!m_device.ShouldClose())
	{
		m_device.BeginFrame();
		m_imguiRenderer.BeginFrame();
		
		HandleInput();
		
		m_renderer.Render();
		
		DrawImGuiWindow();
		
		m_imguiRenderer.EndFrame();
		m_device.EndFrame();
	}
}

void Demo::HandleInput()
{
	Input&	   input		   = m_device.GetInput();
	Camera&	   camera		   = m_renderer.GetCamera();
	Transform& cameraTransform = camera.GetTransform();

	static constexpr float camera_speed		 = 5.0f;
	static constexpr float mouse_sensitivity = 0.1f;

	static float yaw   = 0.0f;
	static float pitch = 0.0f;

	float deltaTime = m_device.GetDeltaTime();

	if (input.GetKeyboardKey(Key::W))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) +
									deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::S))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) -
									deltaTime * camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::A))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) +
									deltaTime * camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::D))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) -
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

		yaw += delta.x * mouse_sensitivity;
		pitch -= delta.y * mouse_sensitivity;
		pitch = glm::clamp(pitch, -89.0f, 89.0f);

		glm::quat qYaw	 = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

		cameraTransform.SetRotation(glm::normalize(qYaw * qPitch));
	}
}

void Demo::DrawImGuiWindow()
{
	ImGui::Begin("Config");
	
	static float smoothFPS = 0.0f;
	static constexpr float fps_alpha = 0.05f; // lower = smoother, higher = more responsive
	
	const float dt = std::max(0.0001f, m_device.GetDeltaTime());
	const float currentFPS = 1.0f / dt;
	if (smoothFPS == 0.0f)
		smoothFPS = currentFPS;
	else
		smoothFPS += fps_alpha * (currentFPS - smoothFPS);
	
	ImGui::Text("%.1f fps", smoothFPS);
	
	ImGui::End();
}
