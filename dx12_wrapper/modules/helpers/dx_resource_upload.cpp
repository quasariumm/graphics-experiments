module;

#include <future>
#include "macros.hpp"

module dx_wrapper.helpers.dx_resource_upload;
import dx_wrapper.helpers.dx_format_helpers;
import dx_wrapper.core;

DxResourceUpload::DxResourceUpload(ID3D12Device2* device)
	: m_device{device}, m_commandListType{D3D12_COMMAND_LIST_TYPE_DIRECT}, m_began{false}
{
	if (!device)
		Log::Critical("Device is nullptr");

	D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
	if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
	{
		m_typedUAVLoadAdditionalFormats = options.TypedUAVLoadAdditionalFormats != 0;
		m_standardSwizzle64KBSupported	= options.StandardSwizzle64KBSupported != 0;
	}
}

void DxResourceUpload::Begin(const D3D12_COMMAND_LIST_TYPE commandType)
{
	if (m_began)
	{
		Log::Warning("Resource upload already began. Ignoring...");
		return;
	}

	switch (commandType)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	case D3D12_COMMAND_LIST_TYPE_COPY:
		break;

	default:
		Log::Critical("DxResourceUpload only supports Direct, Compute, and Copy command queues\n");
	}

	CheckHR(m_device->CreateCommandAllocator(commandType, IID_PPV_ARGS(m_commandAllocator.ReleaseAndGetAddressOf())));
	CheckHR(m_commandAllocator->SetName(L"DxResourceUpload Command Allocator"));

	CheckHR(m_device->CreateCommandList(1,
										commandType,
										m_commandAllocator.Get(),
										nullptr,
										IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
	CheckHR(m_commandList->SetName(L"DxResourceUpload Command List"));

	m_commandListType = commandType;
	m_began			  = true;
}

void DxResourceUpload::Upload(ID3D12Resource* resource, uint32_t subresourceIndexStart, const D3D12_SUBRESOURCE_DATA* subRes,
							  uint32_t numSubresources)
{
	if (!m_began)
		Log::Critical("Can't call Upload on a closed DxResourceUpload.");

	if (!resource || !subRes || !numSubresources)
		Log::Critical("Resource/subresource are null");

	const UINT64 uploadSize = GetRequiredIntermediateSize(resource, subresourceIndexStart, numSubresources);

	const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	const auto					  resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	// Create a temporary buffer
	ComPtr<ID3D12Resource> scratchResource = nullptr;
	CheckHR(m_device->CreateCommittedResource(&heapProps,
											  D3D12_HEAP_FLAG_NONE,
											  &resDesc,
											  D3D12_RESOURCE_STATE_GENERIC_READ,
											  nullptr,
											  IID_PPV_ARGS(scratchResource.GetAddressOf())));
	CheckHR(scratchResource->SetName(L"DxResourceUpload scratchResource"));

	// Submit resource copy to command list
	UpdateSubresources(m_commandList.Get(), resource, scratchResource.Get(), 0, subresourceIndexStart, numSubresources, subRes);

	// Keep it in a list until End()
	m_resourceList.push_back(scratchResource);
}

std::future<void> DxResourceUpload::End(ID3D12CommandQueue* commandQueue)
{
	if (!m_began)
		Log::Critical("Begin() has not been called.");

	if (!commandQueue)
		Log::Critical("Invalid command queue.");

	CheckHR(m_commandList->Close());

	ID3D12CommandList* const lists[] = {m_commandList.Get()};
	commandQueue->ExecuteCommandLists(1, lists);

	ComPtr<ID3D12Fence> fence;
	CheckHR(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
	CheckHR(fence->SetName(L"DxResourceUpload Fence"));

	const HANDLE gpuCompletedEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (!gpuCompletedEvent)
		Log::Critical("CreateEventEx failed. Error code: {}", GetLastError());

	CheckHR(commandQueue->Signal(fence.Get(), 1ULL));
	CheckHR(fence->SetEventOnCompletion(1ULL, gpuCompletedEvent));

	struct UploadBatch
	{
		ComPtr<ID3D12CommandList>			m_commandList;
		ComPtr<ID3D12Fence>					m_fence;
		HANDLE								m_completeEvent;
		std::vector<ComPtr<ID3D12Resource>> m_resourceList;
	};

	auto* uploadBatch = new UploadBatch{m_commandList, fence, gpuCompletedEvent};
	std::ranges::swap(m_resourceList, uploadBatch->m_resourceList);

	std::future<void> future =
			std::async(std::launch::async,
					   [uploadBatch]()
					   {
						   // Wait on the GPU-complete notification
						   const DWORD wr = WaitForSingleObject(uploadBatch->m_completeEvent, INFINITE);
						   if (wr != WAIT_OBJECT_0)
						   {
							   if (wr == WAIT_FAILED)
								   Log::Critical("WatiForSingleObject failed. Error code: {}", GetLastError());
							   Log::Critical("Error whilst WaitForSingleObject");
						   }

						   // Delete the batch
						   // Because the vectors contain smart-pointers, their destructors will
						   // fire and the resources will be released.
						   delete uploadBatch;
					   });

	// Reset state
	m_commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_began			  = false;
	m_commandList.Reset();
	m_commandAllocator.Reset();

	// Swap above should have cleared these
	if (!m_resourceList.empty())
		Log::Critical("DxResourceUpload Resource list is not empty. This should never happen.");

	return future;
}
