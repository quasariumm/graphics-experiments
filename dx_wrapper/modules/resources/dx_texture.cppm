module;

export module dx_wrapper.resources.dx_texture;
import std;
import dx_wrapper.core;
import dx_wrapper.resources.dx_resource;

export enum class TextureType {
	Invalid, // Invalid/Uninitialised texture
	D1,		 // 1D texture
	D1Array, // 1D Array texture
	D2,		 // 2D texture
	D2Array, // 2D Array texture
	D3,		 // 3D texture
	DCube	 // Cubemap texture
};

export class DxTexture : public DxResource
{

public:

	/**
	 * @brief Constructs a new texture from a file. Also supports DDS files
	 * @param device A reference to the device for uploading to the GPU
	 * @param path The path to the image file
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 * @param additionalFlags Additional flags to pass to the resource creation
	 */
	explicit DxTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips = true,
					   D3D12_RESOURCE_FLAGS additionalFlags = D3D12_RESOURCE_FLAG_NONE);

	/**
	 * @brief Constructs a new texture from raw data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param type The type of texture you wish to create
	 * @param format The internal format.
	 * @param width The width in pixels of the image or one slice/face
	 * @param height [optional, default = 1] The height in pixels of the image or one slice/face
	 * @param depth [optional, default = 1] The depth in pixels of the image or the array
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 * @param additionalFlags Additional flags to pass to the resource creation
	 * @attention This function does NOT convert between formats. Write your own conversion shader for that.
	 * @attention This function determines the data size from the width, height and format. If your data is not big enough, this
	 * will break. Use with caution.
	 */
	explicit DxTexture(const DxDevice& device, const std::byte* data, TextureType type, DXGI_FORMAT format, std::uint32_t width,
					   std::uint32_t height = 1, std::uint32_t depth = 1, bool generateMips = true,
					   D3D12_RESOURCE_FLAGS additionalFlags = D3D12_RESOURCE_FLAG_NONE);

	/**
	 * @brief Constructs a new empty texture
	 * @param device A reference to the device for uploading to the GPU
	 * @param type The type of texture you wish to create
	 * @param format The internal format.
	 * @param width The width in pixels of the image or one slice/face
	 * @param height [optional, default = 1] The height in pixels of the image or one slice/face
	 * @param depth [optional, default = 1] The depth in pixels of the image or the array
	 * @param allocateMips [optional, default = true] Whether to generate mipmaps
	 * @param additionalFlags Additional flags to pass to the resource creation
	 * generate ones for other mips/slices
	 */
	explicit DxTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t width,
					   std::uint32_t height = 1, std::uint32_t depth = 1, bool allocateMips = true,
					   D3D12_RESOURCE_FLAGS additionalFlags = D3D12_RESOURCE_FLAG_NONE);

	/**
	 * @brief Constructs a new texture from encoded data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param size The size of the raw data
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 * @param additionalFlags Additional flags to pass to the resource creation
	 * @attention If you already loaded the data through STB or aky akin library, please use the other data-based constructor
	 */
	explicit DxTexture(const DxDevice& device, const std::byte* data, std::size_t size, bool generateMips = true,
					   D3D12_RESOURCE_FLAGS additionalFlags = D3D12_RESOURCE_FLAG_NONE);

	~DxTexture() override;

	ID3D12Resource* GetResource() const override;
	ID3D12Resource* operator*() const override;
	ID3D12Resource* operator->() const override;

	/*
	 * Size operations
	 */

	/**
	 * @brief Resizes the texture without affecting the heap indices
	 * @attention This copies the existing data to the top left corner. If you want to clear the texture as well,
	 *		please use @ref DxTexture::ResizeClear(const DxDevice&,std::uint32_t,std::uint32_t)
	 */
	void Resize(const DxDevice& device, std::uint32_t width, std::uint32_t height = 1, std::uint32_t depth = 1);

	/**
	 * @brief Resizes the texture without affecting the heap indices
	 * @attention Clears the texture. If you want to preserve the data,
	 *		please use @ref DxTexture::Resize(const DxDevice&,std::uint32_t,std::uint32_t)
	 */
	void ResizeClear(const DxDevice& device, std::uint32_t width, std::uint32_t height = 1, std::uint32_t depth = 1);

	/*
	 * Getters via the RESOURCE_DESC
	 */

	std::size_t	  GetWidth() const;
	std::size_t	  GetHeight() const;
	std::size_t	  GetDepthOrArraySize() const;
	std::uint8_t  GetNumChannels() const;
	std::uint32_t GetNumMips() const;

	/**
	 * @brief Creates a UAV for a specified mip or slice
	 * @param device A reference to the device
	 * @param mip The mip to create the UAV for
	 * @param sliceMin [optional, default = 0] The slice to create the UAV for. Only used for Array textures
	 * @param sliceMax [optional, default = 0] The slice to create the UAV for. Only used for Array textures
	 */
	void CreateUAV(const DxDevice& device, std::uint32_t mip, std::uint32_t sliceMin = 0, std::uint32_t sliceMax = 0);

	/**
	 * @brief Gets the index on the DescriptorPile of the SRV
	 */
	std::int32_t GetSrvHeapIndex() const;

	/**
	 * @brief Gets a UAV from the list of UAVs bound to this texture
	 * @param idx [optional, default = 0] The index of which you created the UAV
	 * @return The index on the DescriptorPile of the UAV
	 */
	std::int32_t GetUavHeapIndex(std::uint32_t idx = 0) const;

	TextureType GetTextureType() const { return m_textureType; }

	bool IsValid() const;

protected:

	void UpdateOrCreateSrv(const DxDevice& device, bool isCubemap, bool update = false);
	void UpdateOrCreateUav(const DxDevice& device, std::uint32_t mip, std::uint32_t sliceMin, std::uint32_t sliceMax, bool update = false, std::size_t updateIndex = 0);
	
	virtual void DerivedUpdateDescriptors(const DxDevice&) {}

private:
	

	/** Since I don't have a singleton, I need this to de-allocate the descriptor indices */
	const DxDevice* m_device = nullptr;

	TextureType m_textureType = TextureType::Invalid;

	ComPtr<ID3D12Resource> m_resource;

	CD3DX12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
	std::int32_t					  m_srvHeapIndex = -1;

	std::vector<CD3DX12_UNORDERED_ACCESS_VIEW_DESC> m_uavDescs;
	std::vector<std::int32_t>						m_uavHeapIndices{};
};
