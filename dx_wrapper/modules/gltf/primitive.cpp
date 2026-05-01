module;

#include <d3dx12.h>
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <mikktspace.h>

module dx_wrapper.gltf.primitive;
import dx_wrapper.core.log;
import dx_wrapper.helpers.dx_buffer_helpers;

GltfPrimitive::GltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
							 const fastgltf::Primitive& primitive)
{
	auto positionIt = primitive.findAttribute("POSITION");

	if (positionIt == primitive.attributes.end())
	{
		Log::Error("glTF primitive does not have POSITION attribute");
		return;
	}
	if (!primitive.indicesAccessor.has_value())
	{
		Log::Error("No indices assigned to primitive");
		return;
	}

	ProcessVerticesIndices(asset, primitive);

	// Material
	if (!primitive.materialIndex.has_value())
		m_material = std::nullopt;
	else
		m_material = GltfMaterial{device, modelPath, asset, asset.materials[*primitive.materialIndex]};

	// Make DX12 buffers
	CheckHR(CreateStaticBuffer(device,
							   m_vertices,
							   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
							   m_vertexBuffer));

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes	  = m_vertices.size() * sizeof(Vertex);
	m_vertexBufferView.StrideInBytes  = sizeof(Vertex);

	CheckHR(CreateStaticBuffer(device,
							   m_indices,
							   D3D12_RESOURCE_STATE_INDEX_BUFFER,
							   m_indexBuffer));

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes	 = m_indices.size() * sizeof(uint32_t);
	m_indexBufferView.Format		 = DXGI_FORMAT_R32_UINT;
}

const std::vector<Vertex>&		   GltfPrimitive::GetVertices() const { return m_vertices; }
const std::vector<uint32_t>&	   GltfPrimitive::GetIndices() const { return m_indices; }
const std::optional<GltfMaterial>& GltfPrimitive::GetMaterial() const { return m_material; }

void GltfPrimitive::Bind(const DxDevice& device, const uint32_t vertexBufferSlot) const
{
	auto* commandList = device.GetDXDirectComList();
	commandList->IASetVertexBuffers(vertexBufferSlot, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);
}

void GltfPrimitive::ProcessVerticesIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive)
{
	// Indices
	uint32_t numIndices = 0;
	{
		auto& accessor = asset.accessors[*primitive.indicesAccessor];

		numIndices = accessor.count;
		m_indices.resize(accessor.count);
		fastgltf::iterateAccessorWithIndex<uint32_t>(asset,
													 accessor,
													 [&](const uint32_t index, const std::size_t idx)
													 { m_indices[idx] = index; });
	}

	// Position
	{
		auto& accessor = asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];
		if (!accessor.bufferViewIndex.has_value())
			return;

		m_vertices.resize(accessor.count);
		fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
													  accessor,
													  [&](const glm::vec3& pos, const std::size_t idx)
													  { m_vertices[idx].m_position = pos; });
	}

	// Uv0
	if (const auto* uvAttrib = primitive.findAttribute("TEXCOORD_0"); uvAttrib != primitive.attributes.end())
	{
		auto& accessor = asset.accessors[uvAttrib->accessorIndex];
		if (!accessor.bufferViewIndex.has_value())
			return;

		fastgltf::iterateAccessorWithIndex<glm::vec2>(asset,
													  accessor,
													  [&](const glm::vec2& uv0, const std::size_t idx)
													  { m_vertices[idx].m_uv0 = uv0; });
	}

	// Uv1
	if (const auto* uvAttrib = primitive.findAttribute("TEXCOORD_1"); uvAttrib != primitive.attributes.end())
	{
		auto& accessor = asset.accessors[uvAttrib->accessorIndex];
		if (!accessor.bufferViewIndex.has_value())
			return;

		fastgltf::iterateAccessorWithIndex<glm::vec2>(asset,
													  accessor,
													  [&](const glm::vec2& uv1, const std::size_t idx)
													  { m_vertices[idx].m_uv1 = uv1; });
	}

	// Normal
	if (const auto* normalAttrib = primitive.findAttribute("NORMAL"); normalAttrib != primitive.attributes.end())
	{
		auto& accessor = asset.accessors[normalAttrib->accessorIndex];
		if (!accessor.bufferViewIndex.has_value())
			return;

		fastgltf::iterateAccessorWithIndex<glm::vec3>(asset,
													  accessor,
													  [&](const glm::vec3& normal, const std::size_t idx)
													  { m_vertices[idx].m_normal = normal; });
	}
	else
	{
		// Generate flat normals
		static constexpr uint32_t numVertsPerFace = 3;
		const uint32_t			  numFaces		  = numIndices / numVertsPerFace;
		for (uint32_t i = 0; i < numFaces; i++)
		{
			const uint32_t indicesIndex = i * numVertsPerFace;

			const glm::vec3& a = m_vertices[m_indices[indicesIndex]].m_position;
			const glm::vec3& b = m_vertices[m_indices[indicesIndex + 1]].m_position;
			const glm::vec3& c = m_vertices[m_indices[indicesIndex + 2]].m_position;

			const glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

			m_vertices[m_indices[indicesIndex]].m_normal	 = normal;
			m_vertices[m_indices[indicesIndex + 1]].m_normal = normal;
			m_vertices[m_indices[indicesIndex + 2]].m_normal = normal;
		}
	}

	// Tangent
	if (const auto* tangentAttrib = primitive.findAttribute("TANGENT"); tangentAttrib != primitive.attributes.end())
	{
		auto& accessor = asset.accessors[tangentAttrib->accessorIndex];
		if (!accessor.bufferViewIndex.has_value())
			return;

		fastgltf::iterateAccessorWithIndex<glm::vec4>(asset,
													  accessor,
													  [&](const glm::vec4& tangent, const std::size_t idx)
													  { m_vertices[idx].m_tangent = tangent; });
	}
	else
		CalculateTangents();
}

/*
 * Tangent generation
 */
struct GeometryData
{
	std::vector<Vertex>&		 m_vertices;
	const std::vector<uint32_t>& m_indices;
	uint32_t					 m_numIndices;
};

bool				 gInitInterface = false;
SMikkTSpaceInterface gInterface		= {};

inline int GetVertexIndex(const int iFace, const int iVert) { return (iFace * 3) + iVert; }

int GetNumFaces(const SMikkTSpaceContext* pContext)
{
	const GeometryData* geomData = static_cast<GeometryData*>(pContext->m_pUserData);

	return static_cast<int>(geomData->m_numIndices) / 3;
}

int GetNumVerticesOfFace(const SMikkTSpaceContext*, const int) { return 3; }

void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
	const GeometryData* geomData = static_cast<GeometryData*>(pContext->m_pUserData);

	const glm::vec3& position = geomData->m_vertices[geomData->m_indices[GetVertexIndex(iFace, iVert)]].m_position;

	fvPosOut[0] = position.x;
	fvPosOut[1] = position.y;
	fvPosOut[2] = position.z;
}

void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
	const GeometryData* geomData = static_cast<GeometryData*>(pContext->m_pUserData);

	const glm::vec3& normal = geomData->m_vertices[geomData->m_indices[GetVertexIndex(iFace, iVert)]].m_normal;

	fvNormOut[0] = normal.x;
	fvNormOut[1] = normal.y;
	fvNormOut[2] = normal.z;
}

void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
	const GeometryData* geomData = static_cast<GeometryData*>(pContext->m_pUserData);

	const glm::vec2& uv = geomData->m_vertices[geomData->m_indices[GetVertexIndex(iFace, iVert)]].m_uv0;

	fvTexcOut[0] = uv.x;
	fvTexcOut[1] = uv.y;
}

void SetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace,
					const int iVert)
{
	const auto* geomData = static_cast<GeometryData*>(pContext->m_pUserData);

	const uint32_t vertIndex = geomData->m_indices[GetVertexIndex(iFace, iVert)];

	geomData->m_vertices[vertIndex].m_tangent.x = fvTangent[0];
	geomData->m_vertices[vertIndex].m_tangent.y = fvTangent[1];
	geomData->m_vertices[vertIndex].m_tangent.z = fvTangent[2];
	geomData->m_vertices[vertIndex].m_tangent.w = fSign * -1.0f; // Have to invert tangent sign for some reason
}

void GltfPrimitive::CalculateTangents()
{
	if (!gInitInterface)
	{
		gInterface.m_getNumFaces		  = GetNumFaces;
		gInterface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
		gInterface.m_getPosition		  = GetPosition;
		gInterface.m_getNormal			  = GetNormal;
		gInterface.m_getTexCoord		  = GetTexCoord;
		gInterface.m_setTSpaceBasic		  = SetTSpaceBasic;

		gInitInterface = true;
	}

	GeometryData geomData{
			.m_vertices	  = m_vertices,
			.m_indices	  = m_indices,
			.m_numIndices = static_cast<uint32_t>(m_indices.size()),
	};

	const SMikkTSpaceContext context = {
			.m_pInterface = &gInterface,
			.m_pUserData  = &geomData,
	};

	genTangSpaceDefault(&context);
}
