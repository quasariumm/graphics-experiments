module;

#include <d3dx12.h>
#include <filesystem>
#include <memory>
#include <xstring>
#include <wrl/client.h>

#include <ResourceUploadBatch.h>

/*
 * DX Device
 */

module dx_wrapper.core.dx_device;
import dx_wrapper.core.dx_common;
import dx_wrapper.helper.device_resources;
import dx_wrapper.log;

using namespace Microsoft::WRL;

LRESULT WindowProc(const HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
{
	if (!gRegisteredDevices.contains(hwnd)) return DefWindowProc(hwnd, msg, wp, lp);
	
	DxDevice* device = gRegisteredDevices.at(hwnd);

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		device->m_shouldClose = true;
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

DxDevice::DxDevice(const int width, const int height, const LPCSTR title)
	: m_resourceUpload{}, m_windowWidth(width), m_windowHeight(height)
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

	m_deviceResources = DirectX::DeviceResources{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 3};
	m_deviceResources.SetWindow(window, width, height);
	m_deviceResources.CreateDeviceResources();
	m_deviceResources.CreateWindowSizeDependentResources();

	m_resourceUpload= std::make_unique<DirectX::ResourceUploadBatch>(m_deviceResources.GetD3DDevice());
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
}

void DxDevice::EndFrame()
{
	m_deviceResources.Present();
	
	MSG msg{};
	if (::GetMessage(&msg, m_deviceResources.GetWindow(), 0, 0) <= 0)
	{
		m_shouldClose = true;
		return;
	}
	
	::TranslateMessage(&msg);
	::DispatchMessage(&msg);
}
