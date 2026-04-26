module;

#include <d3d12.h>
#include <d3dx12.h>
#include <vector>

export module dx_wrapper.helpers.dx_buffer_helpers;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;

export HRESULT CreateStaticBuffer(DxDevice& device, const void* data, const size_t size,
								  const D3D12_RESOURCE_STATES initialState, ComPtr<ID3D12Resource>& buffer)
{
	// Create
	const auto heapProps	= CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	const HRESULT result = device->CreateCommittedResource(&heapProps,
														   D3D12_HEAP_FLAG_NONE,
														   &resourceDesc,
														   D3D12_RESOURCE_STATE_COMMON,
														   nullptr,
														   IID_PPV_ARGS(&buffer));

	if (FAILED(result))
		return result;

	// Transition
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COMMON, initialState);
	device.GetDXDirectComList()->ResourceBarrier(1, &barrier);

	// Upload
	D3D12_SUBRESOURCE_DATA initData;
	initData.pData		= data;
	initData.RowPitch	= size;
	initData.SlicePitch = initData.RowPitch;
	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(buffer.Get(), 0, &initData, 1);
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	return result;
}

export template <typename T>
HRESULT CreateStaticBuffer(DxDevice& device, const std::vector<T>& data, const D3D12_RESOURCE_STATES initialState,
						   ComPtr<ID3D12Resource>& buffer)
{ return CreateStaticBuffer(device, data.data(), sizeof(T) * data.size(), initialState, buffer); }
