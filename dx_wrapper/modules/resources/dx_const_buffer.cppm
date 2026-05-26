module;

export module dx_wrapper.resources.dx_const_buffer;
import std;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.helpers.dx_buffer_helpers;
import dx_wrapper.core;

export enum class ConstBufferType : std::uint8_t {
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

	DxConstBuffer() = default;
	explicit DxConstBuffer(DxDevice& device, const T* data, const Type type = Type::Static) { Init(device, data, type); }

	ID3D12Resource* GetResource() const override;
	ID3D12Resource* operator*() const override;
	ID3D12Resource* operator->() const override;

	void UpdateData(const DxDevice& device, const T* data);
	void Bind(const DxDevice& device, const T* data, std::uint32_t rootParameterIndex);

private:

	void Init(DxDevice& device, const T* data, Type type = Type::Static);

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
void DxConstBuffer<T>::Init(DxDevice& device, const T* data, const Type type)
{
	m_device = &device;
	m_type	 = type;

	const std::uint32_t maxIter = type == Type::Static ? 1 : 3;

	static const T default_value = T{};
	if (!data)
		data = &default_value;

	for (std::uint32_t i = 0; i < maxIter; i++)
	{
		m_resources.emplace_back();
		CheckHR(CreateStaticBuffer(device, data, sizeof(T), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_resources[i]));
	}
	SetState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::UpdateData(const DxDevice& device, const T* data)
{
	// Set and upload data
	DxResource::SetData(data, sizeof(T));
	Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::Bind(const DxDevice& device, const T* data, const std::uint32_t rootParameterIndex)
{
	UpdateData(device, data);

	device.GetDXDirectComList()->SetGraphicsRootConstantBufferView(rootParameterIndex, GetResource()->GetGPUVirtualAddress());
}
