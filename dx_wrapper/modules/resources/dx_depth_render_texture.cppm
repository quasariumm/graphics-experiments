module;

export module dx_wrapper.resources.dx_depth_render_texture;
import std;
import dx_wrapper.external.directx12;
import dx_wrapper.core.dx_device;
import dx_wrapper.resources.dx_texture;
import dx_wrapper.rendering.dx_descriptor_heap;

export class DxDepthRenderTexture : public DxTexture
{
public:

	/**
	 * @brief Constructs a new texture from a file. Also supports DDS files
	 * @param device A reference to the device for uploading to the GPU
	 * @param path The path to the image file
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateSrv [optional, default = false] Whether to create an SRV descriptor for this render target
	 * generate ones for other mips/slices
	 */
	explicit DxDepthRenderTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips = true,
								  bool generateSrv = false);

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
	 * @param generateSrv [optional, default = false] Whether to create an SRV descriptor for this render target
	 * @attention This function does NOT convert between formats. Write your own conversion shader for that.
	 * @attention This function determines the data size from the width, height and format. If your data is not big enough, this
	 * will break. Use with caution.
	 */
	explicit DxDepthRenderTexture(const DxDevice& device, const std::byte* data, TextureType type, DXGI_FORMAT format,
								  std::uint32_t width, std::uint32_t height = 1, std::uint32_t depth = 1,
								  bool generateMips = true, bool generateSrv = false);

	/**
	 * @brief Constructs a new empty texture
	 * @param device A reference to the device for uploading to the GPU
	 * @param type The type of texture you wish to create
	 * @param format The internal format.
	 * @param width The width in pixels of the image or one slice/face
	 * @param height [optional, default = 1] The height in pixels of the image or one slice/face
	 * @param depth [optional, default = 1] The depth in pixels of the image or the array
	 * @param allocateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateSrv [optional, default = false] Whether to create an SRV descriptor for this render target
	 */
	explicit DxDepthRenderTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t width,
								  std::uint32_t height = 1, std::uint32_t depth = 1, bool allocateMips = true,
								  bool generateSrv = false);

	/**
	 * @brief Constructs a new texture from encoded data
	 * @param device A reference to the device for uploading to the GPU
	 * @param data The raw data for the image
	 * @param size The size of the raw data
	 * @param generateMips [optional, default = true] Whether to generate mipmaps
	 * @param generateSrv [optional, default = false] Whether to create an SRV descriptor for this render target
	 * @attention If you already loaded the data through STB or aky akin library, please use the other data-based constructor
	 */
	explicit DxDepthRenderTexture(const DxDevice& device, const std::byte* data, std::size_t size, bool generateMips = true,
								  bool generateSrv = false);

	~DxDepthRenderTexture() override = default;

	D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const { return m_dsv; }

private:

	void UpdateOrCreateDsv(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t depth, bool update = false);
	
	void DerivedUpdateDescriptors(const DxDevice& device) override;

	DxDescriptorHeap			  m_dsvHeap;
	D3D12_DEPTH_STENCIL_VIEW_DESC m_dsvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE	  m_dsv{};
};
