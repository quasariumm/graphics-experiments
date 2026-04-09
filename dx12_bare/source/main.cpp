import dx_wrapper.core;
import dx_wrapper.rendering;

import dx_bare.rendering.dx_render;
import dx_bare.core.camera;

struct Demo
{
	DxDevice m_device{};
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
	m_renderer.AddModel("../assets/DamagedHelmet/DamagedHelmet.gltf");
	
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
	
	static constexpr float camera_speed = 0.01f;
	
	if (input.GetKeyboardKey(Key::W))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::S))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - camera_speed * cameraTransform.GetForward(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::A))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::D))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::E))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) + camera_speed * cameraTransform.GetRight(TransformSpace::Local));
	if (input.GetKeyboardKey(Key::Q))
		cameraTransform.SetPosition(cameraTransform.GetPosition(TransformSpace::Local) - camera_speed * cameraTransform.GetRight(TransformSpace::Local));
}
