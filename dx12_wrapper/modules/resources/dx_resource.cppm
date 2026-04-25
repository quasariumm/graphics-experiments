module;

#include <cstdint>
#include <d3d12.h>

#include <ResourceUploadBatch.h>

export module dx_wrapper.resources.dx_resource;
import dx_wrapper.core.dx_common;

export using ::ID3D12Resource;

export class DxResource
{
public:

	DxResource()		  = default;
	virtual ~DxResource() = default;

	/*
	 * State
	 */

	virtual ID3D12Resource* GetResource() const = 0;
	virtual ID3D12Resource* operator*() const	= 0;
	virtual ID3D12Resource* operator->() const	= 0;

	DXGI_FORMAT GetFormat() const { return m_format; }
	void		SetFormat(const DXGI_FORMAT format) { m_format = format; }

	D3D12_RESOURCE_STATES GetCurrentState() const { return m_currentState; }
	void				  Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

	/*
	 * Data
	 */

	virtual void Upload(DirectX::ResourceUploadBatch& resourceUpload, ID3D12CommandQueue* commandQueue,
						ID3D12GraphicsCommandList* commandList);

protected:

	// API for inherited classes

	void SetData(void* data, const size_t size)
	{
		m_cpuData	  = data;
		m_cpuDataSize = size;
	}

	void SetState(const D3D12_RESOURCE_STATES newState) { m_currentState = newState; }

private:

	void*  m_cpuData	 = nullptr;
	size_t m_cpuDataSize = 0;

	DXGI_FORMAT			  m_format		 = DXGI_FORMAT_UNKNOWN;
	D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
};
