module;

export module dx_hw_ray.rendering.dx_ray_pipeline;
import std;
import dx_wrapper.core.dx_device;
import dx_wrapper.external.dxc;
import dx_wrapper.external.win32;
import dx_hw_ray.external.dxr;
import dx_wrapper.rendering.dx_rootsignature;

export class DxRayPipeline
{
public:

	DxRayPipeline() = default;

	// Deref
	ID3D12StateObject*			 operator*() const { return m_stateObject; }
	ID3D12StateObject*			 operator->() const { return m_stateObject; }
	ID3D12StateObject*			 GetStateObject() const { return m_stateObject; }
	ID3D12StateObjectProperties* GetStateObjectProperties() const { return m_stateObjectProperties; }
	ID3D12Resource*				 GetShaderBindingTable() const { return m_sbtResource.Get(); }

	/*
	 * Shaders
	 */

	/**
	 * @brief Sets the RayGen shader of this pipeline
	 * @param path The path to the HLSL file to load
	 * @param entryPoint The name of the function to associate the RayGen shader with
	 * @param inputs A list of heap pointers and GPU virtual address pointers to all root signature bind points
	 * @return This pipeline object for function chaining
	 */
	DxRayPipeline& SetRayGenShader(const std::filesystem::path& path, const std::string& entryPoint,
								   const std::vector<void*>& inputs = {});

	/**
	 * @brief Sets the Miss shader of this pipeline
	 * @param path The path to the HLSL file to load
	 * @param entryPoint The name of the function to associate the Miss shader with
	 * @param inputs A list of heap pointers and GPU virtual address pointers to all root signature bind points
	 * @return This pipeline object for function chaining
	 */
	DxRayPipeline& AddMissShader(const std::filesystem::path& path, const std::string& entryPoint,
								 const std::vector<void*>& inputs = {});

	/**
	 * @brief Adds a new hit group to the pipeline
	 * @param hitGroupName The name of the hit group
	 * @param shaderPath The path to the HLSL file to load
	 * @param closestHitEntryPoint The name of the function to associate the ClosestHit shader with.
	 *		This is called on the intersection point closest to the ray origin.
	 * @param anyHitEntryPoint The name of the function to associate the AnyHit shader with.
	 *		This is called on *every* intersection point along the ray.
	 *		Defaults to being a pass-through shader.
	 * @param intersectEntryPoint The name of the function to associate the Intersect shader with.
	 *		Use this to intersect custom geometry.
	 *		Defaults to a triangle intersection shader.
	 * @param inputs A list of heap pointers and GPU virtual address pointers to all root signature bind points
	 * @return This pipeline object for function chaining
	 */
	DxRayPipeline& AddHitGroup(const std::string& hitGroupName, const std::filesystem::path& shaderPath,
							   const std::string& closestHitEntryPoint, const std::string& anyHitEntryPoint = "",
							   const std::string& intersectEntryPoint = "", const std::vector<void*>& inputs = {});

	/** Associates a root signature with one or more entry points */
	DxRayPipeline& AddRootSignatureAssociation(const DxRootSignature&		   rootSignature,
											   const std::vector<std::string>& entryPoints);

	/*
	 * Parameters
	 */

	/** Sets the maximum size of the Payload struct */
	DxRayPipeline& SetMaxPayloadSize(std::size_t byteSize);

	/** Sets the maximum size of the Attributes struct passed to Hit and Miss shaders */
	DxRayPipeline& SetMaxAttributesSize(std::size_t byteSize);

	/** Sets the max bounces */
	DxRayPipeline& SetMaxRecursionDepth(std::size_t depth);

	void Finalize(DxDevice& device, const std::string& name = "Unnamed Ray Pipeline");

	/*
	 * Size getters
	 */

	/** Get the size in bytes of the SBT section dedicated to ray generation programs */
	std::uint32_t GetRayGenSectionSize() const;
	/** Get the size in bytes of one ray generation program entry in the SBT */
	std::uint32_t GetRayGenEntrySize() const;

	/** Get the size in bytes of the SBT section dedicated to miss programs */
	std::uint32_t GetMissSectionSize() const;
	/** Get the size in bytes of one miss program entry in the SBT */
	std::uint32_t GetMissEntrySize() const;

	/** Get the size in bytes of the SBT section dedicated to hit groups */
	std::uint32_t GetHitGroupSectionSize() const;
	/** Get the size in bytes of hit group entry in the SBT */
	std::uint32_t GetHitGroupEntrySize() const;

private:

	void AddLibrary(const std::filesystem::path& path, const std::vector<std::string>& entryPoints);

	void CreateStateObject(const DxDevice& device);
	void ConfigureStateObjectProperties(DxDevice& device);

	// Helpers from CreateStateObject
	void CreateDummyRootSignatures(DxDevice& device);
	void BuildExportList(std::unordered_set<std::wstring>& exports, std::vector<LPCWSTR>& exportList);

	ID3D12StateObject*			 m_stateObject;
	ID3D12StateObjectProperties* m_stateObjectProperties;
	ComPtr<ID3D12Resource>		 m_sbtResource;

	// Stage names
	std::wstring					m_rayGenName;
	std::vector<void*>				m_rayGenInputs{};
	std::vector<std::wstring>		m_missNames;
	std::vector<std::vector<void*>> m_missInputs{};

	/*
	 * Helpers
	 */

	struct Library
	{
		Library() = default;
		Library(const Library& rhs) { *this = rhs; }
		Library& operator=(const Library& rhs);

		ComPtr<IDxcBlob> m_blob = nullptr;

		std::vector<std::wstring>	   m_entryPoints{};
		std::vector<D3D12_EXPORT_DESC> m_exports{};

		D3D12_DXIL_LIBRARY_DESC m_dxilLibDesc{};
	};
	std::unordered_map<std::filesystem::path, Library> m_libraries;

	struct HitGroup
	{
		HitGroup() = default;
		HitGroup(const HitGroup& rhs) { *this = rhs; }
		HitGroup& operator=(const HitGroup& rhs);

		// These are here to keep the LPCWSTR alive
		std::wstring m_name{};
		std::wstring m_closestHit{};
		std::wstring m_anyHit{};
		std::wstring m_intersect{};

		std::vector<void*> m_inputs{};

		D3D12_HIT_GROUP_DESC m_hitDesc{};
	};
	std::vector<HitGroup> m_hitGroups;

	struct RootSignatureAssociation
	{
		RootSignatureAssociation() = default;
		RootSignatureAssociation(const RootSignatureAssociation& rhs) { *this = rhs; }
		RootSignatureAssociation& operator=(const RootSignatureAssociation& rhs);

		ID3D12RootSignature* m_rootSignature		= nullptr;
		ID3D12RootSignature* m_rootSignaturePointer = nullptr;

		std::vector<std::wstring> m_symbols{};
		std::vector<LPCWSTR>	  m_symbolPointers{};

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION m_association = {};
	};
	std::vector<RootSignatureAssociation> m_rootSignatureAssociations;

	// These two need to be here by rules of the raytracing pipeline
	DxRootSignature m_dummyGlobalRootSignature{};
	DxRootSignature m_dummyLocalRootSignature{};

	std::uint32_t m_maxPayloadSizeInBytes	= 0;
	std::uint32_t m_maxAttributeSizeInBytes = 2 * sizeof(float);
	std::uint32_t m_maxRecursionDepth		= 1;

	// Entry size data
	std::uint32_t m_rayGenEntrySize	  = 0;
	std::uint32_t m_missEntrySize	  = 0;
	std::uint32_t m_hitGroupEntrySize = 0;
};
