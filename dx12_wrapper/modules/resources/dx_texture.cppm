module;

#include <d3dx12.h>
#include <filesystem>

export module dx_wrapper.resources.dx_texture;
import dx_wrapper.core;
import dx_wrapper.resources.dx_resource;

export enum class TextureType {
	Invalid, // Invalid/Uninitialised texture
	D1,		 // 1D texture
	D2,		 // 2D texture
	D2Array, // 2D Array texture
	D3,		 // 3D texture
	D3Array, // 3D Array texture
	DCube	 // Cubemap texture
};

export class DxTexture : protected DxResource
{

public:

	DxTexture() = default;

	/**
	 * @brief Constructs a new texture from a file. Also supports DDS files
	 * @param device A reference to the device for uploading to the GPU
	 * @param path The path to the image file
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateUav [optional, default = false] Whether to create a UAV descriptor
	 */
	explicit DxTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips = true, bool generateUav = false);

	/**
	 * @brief Constructs a new texture from raw data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param type The type of texture you wish to create
	 * @param width The width in pixels of the image or one slice/face
	 * @param height the height in pixels of the image ot one slice/face
	 * @param format The internal format.
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateUav [optional, default = false] Whether to create a UAV descriptor
	 * @attention This function does NOT convert between formats. Write your own conversion shader for that.
	 * @attention This function determines the data size from the width, height and format. If your data is not big enough, this
	 * will break. Use with caution.
	 */
	explicit DxTexture(const DxDevice& device, const void* data, TextureType type, uint32_t width, uint32_t height,
					   DXGI_FORMAT format, bool generateMips = true, bool generateUav = false);

	/**
	 * @brief Constructs a new texture from encoded data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param size The size of the raw data
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateUav [optional, default = false] Whether to create a UAV descriptor
	 * @attention If you already loaded the data through STB or aky akin library, please use the other data-based constructor
	 */
	explicit DxTexture(const DxDevice& device, const void* data, size_t size, bool generateMips = true, bool generateUav = false);

	~DxTexture() override;

	ID3D12Resource* GetResource() const override;
	ID3D12Resource* operator*() const override;
	ID3D12Resource* operator->() const override;

	int32_t GetSrvHeapIndex() const;
	int32_t GetUavHeapIndex() const;

	bool IsValid() const;

private:

	void GenerateDescriptors(const DxDevice& device, bool generateUav, bool isCubemap);

	/** Since I don't have a singleton, I need this to de-allocate the descriptor indices */
	const DxDevice* m_device = nullptr;

	TextureType m_textureType = TextureType::Invalid;

	ComPtr<ID3D12Resource> m_resource;
	int32_t				   m_srvHeapIndex = -1;
	int32_t				   m_uavHeapIndex = -1;
};
