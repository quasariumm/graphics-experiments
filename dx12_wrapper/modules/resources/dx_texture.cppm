module;

#include <d3dx12.h>
#include <filesystem>

export module dx_wrapper.resources.dx_texture;
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

export class DxTexture : protected DxResource
{

public:

	DxTexture(const DxTexture&)			   = delete;
	DxTexture& operator=(const DxTexture&) = delete;
	DxTexture(DxTexture&&)				   = delete;
	DxTexture& operator=(DxTexture&&)	   = delete;

	/**
	 * @brief Constructs a new texture from a file. Also supports DDS files
	 * @param device A reference to the device for uploading to the GPU
	 * @param path The path to the image file
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 */
	explicit DxTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips = true,
					   bool _internal = false);

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
	 * @attention This function does NOT convert between formats. Write your own conversion shader for that.
	 * @attention This function determines the data size from the width, height and format. If your data is not big enough, this
	 * will break. Use with caution.
	 */
	explicit DxTexture(const DxDevice& device, const void* data, TextureType type, DXGI_FORMAT format, uint32_t width,
					   uint32_t height = 1, uint32_t depth = 1, bool generateMips = true, bool _internal = false);

	/**
	 * @brief Constructs a new empty texture
	 * @param device A reference to the device for uploading to the GPU
	 * @param type The type of texture you wish to create
	 * @param format The internal format.
	 * @param width The width in pixels of the image or one slice/face
	 * @param height [optional, default = 1] The height in pixels of the image or one slice/face
	 * @param depth [optional, default = 1] The depth in pixels of the image or the array
	 * @param allocateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 */
	explicit DxTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, uint32_t width, uint32_t height = 1,
					   uint32_t depth = 1, bool allocateMips = true, bool _internal = false);

	/**
	 * @brief Constructs a new texture from encoded data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param size The size of the raw data
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * generate ones for other mips/slices
	 * @attention If you already loaded the data through STB or aky akin library, please use the other data-based constructor
	 */
	explicit DxTexture(const DxDevice& device, const void* data, size_t size, bool generateMips = true, bool _internal = false);

	~DxTexture() override;

	ID3D12Resource* GetResource() const override;
	ID3D12Resource* operator*() const override;
	ID3D12Resource* operator->() const override;

	/**
	 * @brief Creates a UAV for a specified mip or slice
	 * @param device A reference to the device
	 * @param mip The mip to create the UAV for
	 * @param sliceMin [optional, default = 0] The slice to create the UAV for. Only used for Array textures
	 * @param sliceMax [optional, default = 0] The slice to create the UAV for. Only used for Array textures
	 */
	void CreateUAV(const DxDevice& device, uint32_t mip, uint32_t sliceMin = 0, uint32_t sliceMax = 0);

	/**
	 * @brief Gets the index on the DescriptorPile of the SRV
	 */
	int32_t GetSrvHeapIndex() const;

	/**
	 * @brief Gets a UAV from the list of UAVs bound to this texture
	 * @param idx [optional, default = 0] The index of which you created the UAV
	 * @return The index on the DescriptorPile of the UAV
	 */
	int32_t GetUavHeapIndex(uint32_t idx = 0) const;

	TextureType GetTextureType() const { return m_textureType; }

	bool IsValid() const;

protected:

	void GenerateDescriptors(const DxDevice& device, bool generateSrv, bool isCubemap);

private:

	/** Since I don't have a singleton, I need this to de-allocate the descriptor indices */
	const DxDevice* m_device = nullptr;

	TextureType m_textureType = TextureType::Invalid;

	ComPtr<ID3D12Resource> m_resource;
	int32_t				   m_srvHeapIndex = -1;
	std::vector<int32_t>   m_uavHeapIndices{};
};
