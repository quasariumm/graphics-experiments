module;

#include <d3d12.h>
#include <d3dx12.h>

module dx_wrapper.resources.dx_resource;

void DxResource::Upload(DxDevice& device)
{
	if (!m_cpuData)
		return;

	auto& resourceUpload = device.GetResourceUpload();
	resourceUpload.Begin();

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData	   = m_cpuData;
	subResourceData.RowPitch   = m_cpuDataSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	const D3D12_RESOURCE_STATES oldState = m_currentState;
	Transition(device, D3D12_RESOURCE_STATE_COPY_DEST);
	resourceUpload.Upload(GetResource(), 0, &subResourceData, 1);
	Transition(device, oldState);

	resourceUpload.End(device.GetDXDirectComQueue());
}

void DxResource::Transition(const DxDevice& device, const D3D12_RESOURCE_STATES newState)
{
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(), m_currentState, newState);
	device.GetDXDirectComList()->ResourceBarrier(1, &barrier);
	m_currentState = newState;
}
