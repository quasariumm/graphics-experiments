module;

export module dx_imgui.imgui_renderer;
import dx_wrapper.core.dx_device;

export class ImGuiRenderer final
{
	
public:
	
	explicit ImGuiRenderer(DxDevice* device);
	~ImGuiRenderer();
	
	void BeginFrame() const;
	void EndFrame() const;
	
private:
	
	DxDevice* m_device = nullptr;
	
};