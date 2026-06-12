import dx_wrapper.core;
import dx_meshlets.gltf.model;
import dx_meshlets.rendering.dx_renderer;

struct Demo
{
	explicit Demo(const DxDevice::DebugLayerMode debugLayerMode = DxDevice::DebugLayerMode::Enabled)
		: m_device{1920, 1080, "DXR Meshlets", D3D_FEATURE_LEVEL_12_2, debugLayerMode}, m_renderer{&m_device}
	{}
	
	DxDevice   m_device;
	DxRenderer m_renderer;

	void Run();
	void HandleInput();
};

int main(int argc, char** argv)
{
	Demo demo{DxDevice::GetDebugLayerModeFromArgs(argc, argv)};
	demo.Run();
	return 0;
}

void Demo::Run()
{
	m_renderer.AddModel("../dx_bare/assets/Sponza/Sponza.gltf");

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
	m_renderer.GetCamera().HandleInput(m_device);
}
