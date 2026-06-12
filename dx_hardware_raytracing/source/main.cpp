import dx_wrapper.core;
import std;
import dx_hw_ray.rendering.dx_render;
import dx_hw_ray.rendering.imgui_renderer;
import dx_hw_ray.external.imgui;

struct Demo
{
	explicit Demo(const std::span<char*>& args)
		: m_device{1920, 1080, "DXR Hardware Raytracing", D3D_FEATURE_LEVEL_12_2, DxDevice::DebugLayerMode::Enabled, args}, m_renderer{&m_device},
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
	Demo demo{std::span{argv, argv + argc}};
	demo.Run();
	return 0;
}

void Demo::Run()
{
	if (!m_device.SupportsRaytracing())
		Log::Critical("You need raytracing support to run this demo");

	m_renderer.AddModel("../dx_bare/assets/Sponza/Sponza.gltf");
	// m_renderer.AddModel("../dx_bare/assets/Bistro/BistroExterior.gltf");

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
	m_renderer.GetCamera().HandleInput(m_device);
}

void Demo::DrawImGuiWindow()
{
	ImGui::Begin("Config");
	
	m_renderer.Inspector();
	
	ImGui::End();
}
