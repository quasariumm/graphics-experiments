module;

/*
 * DX Device
 */

module dx_wrapper.core.dx_device;

import std;

import dx_wrapper.external.device_resources;
import dx_wrapper.external.win32;
import dx_wrapper.external.directx12;

import dx_wrapper.core;
import dx_wrapper.rendering;

#ifdef TESTPLATE_HAS_IMGUI
extern "C++" LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // NOLINT
#endif

extern "C" LRESULT InputWindowProc(HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp);

LRESULT WindowProc(HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
{
	if (!registered_devices.contains(hwnd))
		return DefWindowProc(hwnd, msg, wp, lp);

#ifdef TESTPLATE_HAS_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
		return true;
#endif

	DxDevice* device = registered_devices.at(hwnd);

	switch (msg)
	{
	/*
	 * Window
	 */
	case WM::Destroy:
		PostQuitMessage(0);
		device->m_shouldClose = true;
		break;
	case WM::Size:
		device->SetWindowSize(LOWORD(lp), HIWORD(lp));
		break;
	default:
		break;
	}
	
	InputWindowProc(hwnd, msg, wp, lp);

	return DefWindowProc(hwnd, msg, wp, lp);
}

DxDevice::DxDevice(const int width, const int height, const LPCSTR title) : m_windowWidth(width), m_windowHeight(height)
{
	// Needed for WIC textures
	CheckHR(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	// Create the window
	m_windowClass = {sizeof(m_windowClass)};

	m_windowClass.lpfnWndProc	= WindowProc;
	m_windowClass.hInstance		= GetModuleHandle(nullptr);
	m_windowClass.lpszClassName = "DX Wrapper Window";
	m_windowClass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	::RegisterClassEx(&m_windowClass);

	RECT rc{0, 0, width, height};
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND window = ::CreateWindowEx(0,
								   "DX Wrapper Window",
								   title,
								   WS_OVERLAPPEDWINDOW,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   rc.right - rc.left,
								   rc.bottom - rc.top,
								   nullptr,
								   nullptr,
								   m_windowClass.hInstance,
								   nullptr);
	registered_devices.emplace(window, this);

	::ShowWindow(window, SW_SHOW);

	m_deviceResources = DirectX::DeviceResources{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
												 DXGI_FORMAT_D32_FLOAT,
												 3,
												 D3D_FEATURE_LEVEL_11_0,
												 DirectX::DeviceResources::c_AllowTearing};
	m_deviceResources.SetWindow(window, width, height);
	m_deviceResources.CreateDeviceResources();
	m_deviceResources.CreateWindowSizeDependentResources();

	m_resourceUpload = std::make_unique<DxResourceUpload>(m_deviceResources.GetD3DDevice());
	m_descriptorPile = std::make_unique<DxDescriptorPile>(GetDXDevice(), max_shader_descriptors);
	m_resourceBank	 = std::make_unique<ResourceBank>();

	// Open the command list for any initialisers
	m_deviceResources.Prepare();
	ID3D12DescriptorHeap* const heaps[1] = {m_descriptorPile->GetHeap()};
	GetDXDirectComList()->SetDescriptorHeaps(1, heaps);

	m_commandListOpened = true;
}

DxDevice::~DxDevice()
{
	m_deviceResources.WaitForGpu();
	::DestroyWindow(m_deviceResources.GetWindow());
	::UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

void DxDevice::SetWindowTitle(const std::string& title) const { SetWindowText(m_deviceResources.GetWindow(), title.c_str()); }

void DxDevice::SetWindowSize(int width, int height)
{
	m_windowWidth  = width;
	m_windowHeight = height;
	m_deviceResources.WindowSizeChanged(width, height);
}

void DxDevice::SetWindowPos(int xpos, int ypos) const
{ ::SetWindowPos(m_deviceResources.GetWindow(), HWND_TOPMOST, xpos, ypos, m_windowWidth, m_windowHeight, SWP_NOACTIVATE); }

void DxDevice::SetWindowFullscreenState(bool active, int width, int height, int xpos, int ypos)
{
	const HWND hwnd = m_deviceResources.GetWindow();
	if (active)
	{
		// Save current placement so we can restore it
		::GetWindowPlacement(hwnd, &m_windowedPlacement);

		::SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		::SetWindowPos(hwnd, HWND_TOP, xpos, ypos, width, height, SWP_FRAMECHANGED | SWP_NOACTIVATE);
		::ShowWindow(hwnd, SW_MAXIMIZE);
	}
	else
	{
		::SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		::SetWindowPlacement(hwnd, &m_windowedPlacement);
		::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
		::ShowWindow(hwnd, SW_NORMAL);
	}
}

void DxDevice::SetWindowIcon(const std::filesystem::path& filename) const
{
	const HWND hwnd = m_deviceResources.GetWindow();

	auto hIconSmall = static_cast<HICON>(::LoadImage(nullptr,
													 filename.string().c_str(),
													 IMAGE_ICON,
													 GetSystemMetrics(SM_CXSMICON),
													 GetSystemMetrics(SM_CYSMICON),
													 LR_LOADFROMFILE));

	auto hIconBig = static_cast<HICON>(::LoadImage(nullptr,
												   filename.string().c_str(),
												   IMAGE_ICON,
												   GetSystemMetrics(SM_CXICON),
												   GetSystemMetrics(SM_CYICON),
												   LR_LOADFROMFILE));

	if (!hIconBig || !hIconSmall)
		return;

	::SendMessage(hwnd, WM::SetIcon, ICON_SMALL, (LPARAM)hIconSmall);
	::SendMessage(hwnd, WM::SetIcon, ICON_BIG, (LPARAM)hIconBig);
}

void DxDevice::SetWindowCursorState(bool active) const
{
	const HWND hwnd = m_deviceResources.GetWindow();
	if (active)
	{
		::ShowCursor(TRUE);
		::ClipCursor(nullptr); // release confinement
	}
	else
	{
		::ShowCursor(FALSE);
		RECT rc;
		::GetClientRect(hwnd, &rc);
		::MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&rc), 2);
		::ClipCursor(&rc); // confine to window so mouse doesn't drift off
	}
}

void DxDevice::BeginFrame()
{
	if (m_commandListOpened)
	{
		m_deviceResources.Present();
		m_commandListOpened = false;
	}

	m_lastFrameTime = Clock::now();

	m_deviceResources.Prepare();

	auto*	   commandList = m_deviceResources.GetCommandList();
	const auto viewport	   = m_deviceResources.GetScreenViewport();
	const auto rect		   = m_deviceResources.GetScissorRect();
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);

	ID3D12DescriptorHeap* const heaps[1] = {m_descriptorPile->GetHeap()};
	commandList->SetDescriptorHeaps(1, heaps);
}

void DxDevice::EndFrame()
{
	try
	{
		m_deviceResources.Present();
	}
	catch (std::exception&)
	{
		m_deviceResources.HandleDeviceLost();
	}

	MSG msg{};
	::PeekMessage(&msg, m_deviceResources.GetWindow(), 0, 0, PM_REMOVE);

	if (msg.message == WM::Quit)
	{
		m_shouldClose = true;
		return;
	}

	::TranslateMessage(&msg);
	::DispatchMessage(&msg);

	m_input.Update();
	m_deltaTime = std::chrono::duration<float>(Clock::now() - m_lastFrameTime).count();
}
