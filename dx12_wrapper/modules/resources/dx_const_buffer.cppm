module;

#include <d3d12.h>
#include <vector>

#include <BufferHelpers.h>

export module dx_wrapper.resources.dx_const_buffer;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.core;

export enum class ConstBufferType : uint8_t {
	Static,	 // One constant buffer shared over all back buffers
	Dynamic, // One constant buffer per back buffer
};

template <typename T>
concept ConstBufferRequriement = sizeof(T) % 256 == 0;

export template <typename T>
	requires ConstBufferRequriement<T>
class DxConstBuffer final : protected DxResource
{
public:

	using Type = ConstBufferType;

	explicit DxConstBuffer(DxDevice& device, T* data, const Type type = Type::Static) { Init(device, data, type); }

	ID3D12Resource* GetResource() const override;
	ID3D12Resource* operator*() const override;
	ID3D12Resource* operator->() const override;

	void Init(DxDevice& device, T* data, Type type = Type::Static);
	void Bind(const DxDevice& device, T* data, uint32_t rootParameterIndex);

private:

	DxDevice* m_device = nullptr;

	std::vector<ComPtr<ID3D12Resource>> m_resources;

	Type m_type = Type::Static;
};

//====================================
// Impl
//====================================

template <typename T>
	requires ConstBufferRequriement<T>
ID3D12Resource* DxConstBuffer<T>::GetResource() const
{
	switch (m_type)
	{
	case Type::Static:
		return m_resources[0].Get();
	case Type::Dynamic:
		if (!m_device)
			return nullptr;
		return m_resources[m_device->GetCurBackBufferIndex()].Get();
	}
	return nullptr;
}

template <typename T>
	requires ConstBufferRequriement<T>
ID3D12Resource* DxConstBuffer<T>::operator*() const
{ return GetResource(); }

template <typename T>
	requires ConstBufferRequriement<T>
ID3D12Resource* DxConstBuffer<T>::operator->() const
{ return GetResource(); }

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::Init(DxDevice& device, T* data, const Type type)
{
	m_device = &device;
	m_type	 = type;

	const uint32_t maxIter = type == Type::Static ? 1 : 3;

	static T default_value = T{};
	if (!data)
		data = &default_value;

	for (uint32_t i = 0; i < maxIter; i++)
	{
		m_resources.emplace_back();
		device.GetResourceUpload().Begin();
		CheckHR(DirectX::CreateStaticBuffer<T>(*device,
											   device.GetResourceUpload(),
											   data,
											   1,
											   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
											   m_resources[i].GetAddressOf()));
		device.GetResourceUpload().End(device.GetDXDirectComQueue());
	}
	SetState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::Bind(const DxDevice& device, T* data, const uint32_t rootParameterIndex)
{
	// Set and upload data
	DxResource::SetData(data, sizeof(T));
	Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());

	device.GetDXDirectComList()->SetGraphicsRootConstantBufferView(rootParameterIndex, GetResource()->GetGPUVirtualAddress());
}
