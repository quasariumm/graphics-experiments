module;

#include <directx/d3d12.h>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <windef.h>
#include <winuser.h>

/*
 * DX Device
 */

export module core.dx_device;
import helper.device_resources;

export class DxDevice
{

public:
	explicit DxDevice(
		int width = 1920,
		int height = 1080,
		LPCWSTR title = L"DX12 Wrapper Window"
	);

	[[nodiscard]] int GetWidth() const { return m_windowWidth; }
	[[nodiscard]] int GetHeight() const { return m_windowHeight; }
	
	bool ShouldClose() const { return m_shouldClose; }
	void RequestClose() { m_shouldClose = true; }
	
	/*
	 * Window Functions
	 */
	void SetWindowTitle(const std::wstring& title) const;
	void SetWindowSize(int width, int height);
	void SetWindowPos(int xpos, int ypos) const;
	void SetWindowFullscreenState(bool active, int width, int height, int xpos, int ypos);
	void SetWindowIcon(const std::filesystem::path& filename) const;
	void SetWindowCursorState(bool active) const;

	ID3D12Device* GetDXDevice() { return m_deviceResources->GetD3DDevice(); }
	ID3D12GraphicsCommandList* GetDXDirectComList() { return m_deviceResources->GetCommandList(); }
	ID3D12CommandQueue* GetDXDirectComQueue() { return m_deviceResources->GetCommandQueue(); }
	UINT GetCurBackBufferIndex() const { return m_deviceResources->GetCurrentFrameIndex(); }
	
	void BeginFrame();
	void EndFrame();
	
private:
	friend LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
	
	std::unique_ptr<DX::DeviceResources> m_deviceResources;
	int m_windowWidth;
	int m_windowHeight;
	
	WINDOWPLACEMENT	m_windowedPlacement;
	
	bool m_shouldClose = false;
};

inline std::unordered_map<HWND, DxDevice*> gRegisteredDevices = {};
