// NOLINTBEGIN
module;

#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

export module dx_imgui.external.imgui_impl;
export import dx_imgui.external.imgui;

export {
	// ----- Types -----

	using ::ImDrawData;
	using ::ImTextureData;
	using ::ImGui_ImplDX12_InitInfo;
	using ::ImGui_ImplDX12_RenderState;

	// ----- Functions -----

	using ::ImGui_ImplDX12_CreateDeviceObjects;
	using ::ImGui_ImplDX12_InvalidateDeviceObjects;
	using ::ImGui_ImplDX12_Init;
	using ::ImGui_ImplDX12_NewFrame;
	using ::ImGui_ImplDX12_RenderDrawData;
	using ::ImGui_ImplDX12_Shutdown;
	using ::ImGui_ImplDX12_UpdateTexture;
}
// NOLINTEND