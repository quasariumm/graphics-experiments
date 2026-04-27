module;

module dx_wrapper.resources.dx_resource;
import dx_wrapper.core;

void DxResource::Transition(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES newState)
{
	if (newState == m_currentState) return;
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(), m_currentState, newState);
	commandList->ResourceBarrier(1, &barrier);
	m_currentState = newState;
}

void DxResource::Upload(DxResourceUpload& resourceUpload, ID3D12CommandQueue* commandQueue,
						ID3D12GraphicsCommandList* commandList)
{
	if (!m_cpuData)
		return;

	resourceUpload.Begin();

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData	   = m_cpuData;
	subResourceData.RowPitch   = m_cpuDataSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	const D3D12_RESOURCE_STATES oldState = m_currentState;
	Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	resourceUpload.Upload(GetResource(), 0, &subResourceData, 1);
	Transition(commandList, oldState);

	resourceUpload.End(commandQueue);
}
