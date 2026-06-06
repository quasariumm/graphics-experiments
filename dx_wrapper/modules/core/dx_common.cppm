module;

#include <d3d12.h>

export module dx_wrapper.core.dx_common;
import std;
import dx_wrapper.core.log;
export import dx_wrapper.core.common;
export import dx_wrapper.external.win32;
export import dx_wrapper.external.directx12;

export inline constexpr std::uint32_t max_shader_descriptors = 16384;

export inline ID3D12Device5* dred_query_device = nullptr;

export void CheckHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		if ((hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) && dred_query_device)
		{
			const HRESULT removedReason = dred_query_device->GetDeviceRemovedReason();

			ComPtr<ID3D12DeviceRemovedExtendedData> dred;
			if (SUCCEEDED(dred_query_device->QueryInterface(IID_PPV_ARGS(dred.GetAddressOf()))))
			{
				D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT breadcrumbs{};
				D3D12_DRED_PAGE_FAULT_OUTPUT	   pageFault{};
				dred->GetAutoBreadcrumbsOutput(&breadcrumbs);
				dred->GetPageFaultAllocationOutput(&pageFault);

				Log::Critical("Device removed: reason=0x{:08X} page fault VA=0x{:016X}",
							  static_cast<unsigned int>(removedReason),
							  pageFault.PageFaultVA);

				// Walk breadcrumbs
				auto* node = breadcrumbs.pHeadAutoBreadcrumbNode;
				while (node)
				{
					Log::Critical("  Breadcrumb: {} completed {}/{} ops",
								  node->pCommandListDebugNameA ? node->pCommandListDebugNameA : "<unnamed>",
								  *node->pLastBreadcrumbValue,
								  node->BreadcrumbCount);
					node = node->pNext;
				}
			}
		}
		else
		{
			CHAR  errMsgBuf[256];
			DWORD errMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
											nullptr,
											hr,
											0,
											errMsgBuf,
											sizeof(errMsgBuf),
											nullptr);

			if (errMsgLen != 0)
				Log::Critical("{}", errMsgBuf);
			else
				Log::Critical("A DirectX 12 error has occurred: 0x{:08X}", static_cast<unsigned int>(hr));
		}
	}
}

export D3D12_RESOURCE_DESC GetDesc(ID3D12Resource* resource)
{
#ifdef _MSC_VER
	return resource->GetDesc();
#else
	D3D12_RESOURCE_DESC desc;
	resource->GetDesc(&desc);
	return desc;
#endif
}
