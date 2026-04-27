module;

#include <cstdint>
#include <future>

export module dx_wrapper.helpers.dx_resource_upload;
import dx_wrapper.core.dx_common;

/**
 * Asynchronously uploads buffers
 * Code mostly from https://github.com/microsoft/DirectXTK12/blob/main/Inc/ResourceUploadBatch.h
 */
export class DxResourceUpload final
{
public:

	explicit DxResourceUpload(ID3D12Device2* device);

	// Call this before your multiple calls to Upload.
	void Begin(D3D12_COMMAND_LIST_TYPE commandType = D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Asynchronously uploads a resource. The memory in subRes is copied.
	void Upload(ID3D12Resource* resource, uint32_t subresourceIndexStart, const D3D12_SUBRESOURCE_DATA* subRes,
				uint32_t numSubresources);

	// Submits all the uploads to the driver.
	// No more uploads can happen after this call until Begin is called again.
	// This returns a handle to an event that can be waited on.
	std::future<void> End(ID3D12CommandQueue* commandQueue);

	DxResourceUpload(DxResourceUpload&&)				 = default;
	DxResourceUpload& operator=(DxResourceUpload&&)		 = default;
	DxResourceUpload(DxResourceUpload const&)			 = delete;
	DxResourceUpload& operator=(DxResourceUpload const&) = delete;

private:

	ID3D12Device2*					  m_device;
	ComPtr<ID3D12CommandAllocator>	  m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	std::vector<ComPtr<ID3D12Resource>> m_resourceList;

	D3D12_COMMAND_LIST_TYPE m_commandListType;
	bool					m_began;
	bool					m_typedUAVLoadAdditionalFormats;
	bool					m_standardSwizzle64KBSupported;
};
