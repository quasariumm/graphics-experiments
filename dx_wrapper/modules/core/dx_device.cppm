module;

/*
 * DX Device
 */

export module dx_wrapper.core.dx_device;

import std;

import dx_wrapper.external.device_resources;
import dx_wrapper.external.win32;
import dx_wrapper.external.directx12;

import dx_wrapper.core.input;

import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.resources.resource_bank;
import dx_wrapper.helpers.dx_resource_upload;

export class DxDevice
{

public:

	explicit DxDevice(int width = 1920, int height = 1080, LPCSTR title = "DX12 Wrapper Window");
	~DxDevice();

	Input& GetInput() { return m_input; }

	[[nodiscard]] int GetWidth() const { return m_windowWidth; }
	[[nodiscard]] int GetHeight() const { return m_windowHeight; }

	bool ShouldClose() const { return m_shouldClose; }
	void RequestClose() { m_shouldClose = true; }

	/*
	 * Window Functions
	 */
	void SetWindowTitle(const std::string& title) const;
	void SetWindowSize(int width, int height);
	void SetWindowPos(int xpos, int ypos) const;
	void SetWindowFullscreenState(bool active, int width, int height, int xpos, int ypos);
	void SetWindowIcon(const std::filesystem::path& filename) const;
	void SetWindowCursorState(bool active) const;

	DxResourceUpload& GetResourceUpload() const { return *m_resourceUpload; }
	DxDescriptorPile& GetShaderDescriptorPile() const { return *m_descriptorPile; }
	ResourceBank&	  GetResourceBank() const { return *m_resourceBank; }

	ID3D12Device2*				  operator*() const { return m_deviceResources.GetD3DDevice(); }
	ID3D12Device2*				  operator->() const { return m_deviceResources.GetD3DDevice(); }
	ID3D12Device2*				  GetDXDevice() const { return m_deviceResources.GetD3DDevice(); }
	ID3D12GraphicsCommandList*	  GetDXDirectComList() const { return m_deviceResources.GetCommandList(); }
	ID3D12CommandQueue*			  GetDXDirectComQueue() const { return m_deviceResources.GetCommandQueue(); }
	ID3D12Resource*				  GetRenderTarget() const { return m_deviceResources.GetRenderTarget(); }
	ID3D12Resource*				  GetDepthTarget() const { return m_deviceResources.GetDepthStencil(); }
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_deviceResources.GetRenderTargetView(); }
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return m_deviceResources.GetDepthStencilView(); }
	UINT						  GetCurBackBufferIndex() const { return m_deviceResources.GetCurrentFrameIndex(); }
	float						  GetDeltaTime() const { return m_deltaTime; }
	HWND						  GetWindow() const { return m_deviceResources.GetWindow(); }

	void BeginFrame();
	void EndFrame();

private:

	friend LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);

	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point m_lastFrameTime;
	float			  m_deltaTime = 0.f;

	Input m_input{};

	DirectX::DeviceResources		  m_deviceResources;
	std::unique_ptr<DxResourceUpload> m_resourceUpload;
	std::unique_ptr<DxDescriptorPile> m_descriptorPile;
	std::unique_ptr<ResourceBank>	  m_resourceBank;

	int m_windowWidth;
	int m_windowHeight;

	// Win32 stuff
	WNDCLASSEX m_windowClass;
	WINDOWPLACEMENT m_windowedPlacement;

	bool m_shouldClose		 = false;
	bool m_commandListOpened = false;
};

inline std::unordered_map<HWND, DxDevice*> registered_devices = {}; // NOLINT
