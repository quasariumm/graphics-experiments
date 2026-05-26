// NOLINTBEGIN
module;

#define IMGUI_IMPL_API extern "C++"
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

export module dx_hw_ray.external.imgui_impl;
export import dx_hw_ray.external.imgui;

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
	
	using ::ImGui_ImplWin32_Init;
	using ::ImGui_ImplWin32_Shutdown;
	using ::ImGui_ImplWin32_NewFrame;
	using ::ImGui_ImplWin32_EnableDpiAwareness;
	using ::ImGui_ImplWin32_GetDpiScaleForHwnd;
	using ::ImGui_ImplWin32_GetDpiScaleForMonitor;
	using ::ImGui_ImplWin32_EnableAlphaCompositing;
	
	extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
// NOLINTEND