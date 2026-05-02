import dx_wrapper.core;
import dx_imgui.imgui_renderer;
import dx_imgui.external.imgui;

int main()
{
	DxDevice device{};
	const ImGuiRenderer imGuiRenderer{&device};
	
	while (!device.ShouldClose())
	{
		device.BeginFrame();
		imGuiRenderer.BeginFrame();
		
		static bool demoWindowOpen = true;
		ImGui::ShowDemoWindow(&demoWindowOpen);
		
		imGuiRenderer.EndFrame();
		device.EndFrame();
	}
	
	return 0;
}