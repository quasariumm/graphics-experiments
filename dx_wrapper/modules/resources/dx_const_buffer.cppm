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
concept ConstBufferRequriement = std::is_same_v<std::decay_t<T>, T> && sizeof(T) % 256 == 0;

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

	/**
	 * @brief Copies the given data to the GPU resource.
	 * @param device The device
	 * @param data The data you want to give
	 * @attention For better performance, please use the r-value reference overload if you don't modify the data later.
	 */
	void UpdateData(const DxDevice& device, const T& data);

	/**
	 * @brief Copies the given data to the GPU resource.
	 * @param device The device
	 * @param data The data you want to give
	 */
	void UpdateData(const DxDevice& device, T&& data);

	/**
	 * @brief Copies the given data to the GPU and binds it to a CBV parameter
	 * @param device The device
	 * @param data The data you want to give
	 * @param rootParameterIndex The index in the root parameter list (NOT THE SHADER REGISTER)
	 * @attention For better performance, please use the r-value reference overload if you don't modify the data later.
	 */
	void Bind(const DxDevice& device, const T& data, std::uint32_t rootParameterIndex);

	/**
	 * @brief Copies the given data to the GPU and binds it to a CBV parameter
	 * @param device The device
	 * @param data The data you want to give
	 * @param rootParameterIndex The index in the root parameter list (NOT THE SHADER REGISTER)
	 */
	void Bind(const DxDevice& device, T&& data, std::uint32_t rootParameterIndex);

private:

	void Init(DxDevice& device, const T* data, Type type = Type::Static);

	DxDevice* m_device = nullptr;

	T m_localInstance{};

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
	m_localInstance				 = (data) ? *data : default_value;
	DxResource::SetData(&m_localInstance, sizeof(T));
	
	const auto typeName = std::filesystem::path{typeid(T).name()}.wstring();

	for (std::uint32_t i = 0; i < maxIter; i++)
	{
		m_resources.emplace_back();
		CheckHR(CreateStaticBuffer(device,
								   &m_localInstance,
								   sizeof(T),
								   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
								   m_resources[i]));
		
		const auto resourceName = std::format(L"Constant Buffer ({}/{}, Type: {})", i + 1, maxIter, typeName.c_str());
		CheckHR(m_resources[i]->SetName(resourceName.c_str()));
	}
	SetState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::UpdateData(const DxDevice& device, T&& data)
{
	// Set and upload data
	m_localInstance = std::move(data);
	DxResource::SetData(&m_localInstance, sizeof(T));
	Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::UpdateData(const DxDevice& device, const T& data)
{
	// Set and upload data
	m_localInstance = data;
	DxResource::SetData(&m_localInstance, sizeof(T));
	Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::Bind(const DxDevice& device, T&& data, const std::uint32_t rootParameterIndex)
{
	UpdateData(device, data);

	device.GetDXDirectComList()->SetGraphicsRootConstantBufferView(rootParameterIndex, GetResource()->GetGPUVirtualAddress());
}

template <typename T>
	requires ConstBufferRequriement<T>
void DxConstBuffer<T>::Bind(const DxDevice& device, const T& data, const std::uint32_t rootParameterIndex)
{
	UpdateData(device, data);

	device.GetDXDirectComList()->SetGraphicsRootConstantBufferView(rootParameterIndex, GetResource()->GetGPUVirtualAddress());
}
