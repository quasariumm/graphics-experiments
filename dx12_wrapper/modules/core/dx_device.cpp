module;

#include <d3dx12.h>
#include <filesystem>
#include <memory>

#include <ResourceUploadBatch.h>
#include <bitset>

/*
 * DX Device
 */

module dx_wrapper.core.dx_device;
import dx_wrapper.external.device_resources;
import dx_wrapper.core;

std::bitset<0xff> current_input_state{}; // NOLINT
glm::vec2 current_mouse_pos{}; // NOLINT
glm::vec2 current_scroll_delta{}; // NOLINT

LRESULT WindowProc(HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
{
	if (!gRegisteredDevices.contains(hwnd))
		return DefWindowProc(hwnd, msg, wp, lp);

	DxDevice* device = gRegisteredDevices.at(hwnd);
	
	float dpi = GetDpiForWindow(hwnd) / 96.0f;

	switch (msg)
	{
	/*
	 * Window
	 */
	case WM_DESTROY:
		PostQuitMessage(0);
		device->m_shouldClose = true;
		break;
	case WM_SIZE:
		device->SetWindowSize(LOWORD(lp), HIWORD(lp));
		break;
	/*
	 * Keyboard
	 */
	case WM_KEYDOWN:
		current_input_state.set(LOWORD(wp), true);
		break;
	case WM_KEYUP:
		current_input_state.set(LOWORD(wp), false);
		break;
	/*
	 * Mouse
	 */
	case WM_LBUTTONDOWN:
		current_input_state.set(static_cast<size_t>(MouseButton::Left), true);
		SetCapture(hwnd);
		break;
	case WM_RBUTTONDOWN:
		current_input_state.set(static_cast<size_t>(MouseButton::Right), true);
		break;
	case WM_MBUTTONDOWN:
		current_input_state.set(static_cast<size_t>(MouseButton::Middle), true);
		break;
	case WM_XBUTTONDOWN:
	{
		const UINT button = GET_XBUTTON_WPARAM(wp);
		current_input_state.set(static_cast<size_t>(button == XBUTTON1 ? MouseButton::X1 : MouseButton::X2), true);
		break;
	}
	case WM_LBUTTONUP:
		current_input_state.set(static_cast<size_t>(MouseButton::Left), false);
		ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		current_input_state.set(static_cast<size_t>(MouseButton::Right), false);
		break;
	case WM_MBUTTONUP:
		current_input_state.set(static_cast<size_t>(MouseButton::Middle), false);
		break;
	case WM_XBUTTONUP:
	{
		const UINT button = GET_XBUTTON_WPARAM(wp);
		current_input_state.set(static_cast<size_t>(button == XBUTTON1 ? MouseButton::X1 : MouseButton::X2), false);
		break;
	}
	case WM_MOUSEMOVE:
		current_mouse_pos.x = static_cast<float>(LOWORD(lp)) / dpi;
		current_mouse_pos.y = static_cast<float>(HIWORD(lp)) / dpi;
		break;
	case WM_MOUSEWHEEL:
		current_scroll_delta.y = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp)) / 120.0f;
		break;
	case WM_MOUSEHWHEEL:
		current_scroll_delta.x = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp)) / 120.0f;
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

DxDevice::DxDevice(const int width, const int height, const LPCSTR title)
	: m_windowWidth(width), m_windowHeight(height)
{
	// Create the window
	WNDCLASSEX wc = {sizeof(wc)};

	wc.lpfnWndProc	 = WindowProc;
	wc.hInstance	 = GetModuleHandle(nullptr);
	wc.lpszClassName = "DX Wrapper Window";
	wc.hCursor		 = LoadCursor(nullptr, IDC_ARROW);
	::RegisterClassEx(&wc);

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
								   wc.hInstance,
								   nullptr);
	gRegisteredDevices.emplace(window, this);

	::ShowWindow(window, SW_SHOW);

	m_deviceResources = DirectX::DeviceResources{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
												 DXGI_FORMAT_D32_FLOAT,
												 3,
												 D3D_FEATURE_LEVEL_11_0,
												 DirectX::DeviceResources::c_AllowTearing};
	m_deviceResources.SetWindow(window, width, height);
	m_deviceResources.CreateDeviceResources();
	m_deviceResources.CreateWindowSizeDependentResources();

	m_resourceUpload = std::make_unique<DirectX::ResourceUploadBatch>(m_deviceResources.GetD3DDevice());
}

DxDevice::~DxDevice() { m_deviceResources.WaitForGpu(); }

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

	::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
	::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
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
	m_deviceResources.Prepare();

	auto*	   commandList = m_deviceResources.GetCommandList();
	const auto viewport	   = m_deviceResources.GetScreenViewport();
	const auto rect		   = m_deviceResources.GetScissorRect();
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
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

	if (msg.message == WM_QUIT)
	{
		m_shouldClose = true;
		return;
	}

	::TranslateMessage(&msg);
	::DispatchMessage(&msg);
	
	m_input.Update();
}
