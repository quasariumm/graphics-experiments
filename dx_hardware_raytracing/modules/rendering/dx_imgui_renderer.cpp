module;

#include <cassert>
#include <cstdint>

module dx_hw_ray.rendering.imgui_renderer;
import dx_wrapper.core.log;
import dx_wrapper.external.directx12;
import dx_hw_ray.external.imgui_impl;

ImGuiRenderer::ImGuiRenderer(DxDevice* device)
{
	m_device = device;
	
	// Setup Dear ImGui context
	ImGui::CheckVersion();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	ImGui::StyleColorsDark();
	
	ImGuiStyle& style = ImGui::GetStyle();
	const float dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(device->GetWindow());
	style.ScaleAllSizes(dpiScale);
	style.FontScaleDpi = dpiScale;
	
	io.Fonts->AddFontDefaultVector();

	ImGui_ImplWin32_EnableDpiAwareness();
	ImGui_ImplWin32_Init(device->GetWindow());

	ImGui_ImplDX12_InitInfo initInfo = {};
	initInfo.Device					 = device->GetDXDevice();
	initInfo.CommandQueue			 = device->GetDXDirectComQueue();
	initInfo.NumFramesInFlight		 = 3;
	initInfo.RTVFormat				 = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Srv Allocator
	initInfo.SrvDescriptorHeap	  = device->GetShaderDescriptorPile().GetHeap();
	initInfo.UserData			  = m_device;
	initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*		info,
									   D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
									   D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle)
	{
		const auto* device = static_cast<DxDevice*>(info->UserData);
		if (!device)
			return;
		const uint32_t idx = device->GetShaderDescriptorPile().Allocate();
		outCpuHandle->ptr  = device->GetShaderDescriptorPile().GetCpuHandleAt(idx).ptr;
		outGpuHandle->ptr  = device->GetShaderDescriptorPile().GetGpuHandleAt(idx).ptr;
	};
	initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*					   info,
									  D3D12_CPU_DESCRIPTOR_HANDLE				   cpuHandle,
									  [[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		const auto* device = static_cast<DxDevice*>(info->UserData);
		if (!device)
			return;

		const size_t cpuDescIndex = device->GetShaderDescriptorPile().GetIndexFromHandle(cpuHandle);
#ifdef _DEBUG
		const size_t gpuDescIndex = device->GetShaderDescriptorPile().GetIndexFromHandle(gpuHandle);
		assert(cpuDescIndex == gpuDescIndex);
#endif

		device->GetShaderDescriptorPile().Free(cpuDescIndex);
	};
	
	ImGui_ImplDX12_Init(&initInfo);
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::BeginFrame() const
{
	if (!m_device) Log::Error("ImGuiRenderer::BeginFrame() called when device is null");
	
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();
	
	// Clear render target
	const D3D12_CPU_DESCRIPTOR_HANDLE targetRTV = m_device->GetRTV();
	static constexpr float rt_clear_color[4] = {0.f, 0.f, 0.f, 1.f};
	m_device->GetDXDirectComList()->ClearRenderTargetView(targetRTV, rt_clear_color, 0, nullptr);
}

void ImGuiRenderer::EndFrame() const
{
	if (!m_device) Log::Error("ImGuiRenderer::EndFrame() called when device is null");
	
	ID3D12GraphicsCommandList* commandList = m_device->GetDXDirectComList();
	const D3D12_CPU_DESCRIPTOR_HANDLE targetRTV = m_device->GetRTV();

	commandList->OMSetRenderTargets(1, &targetRTV, TRUE, nullptr);
	
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_device->GetDXDirectComList());
}