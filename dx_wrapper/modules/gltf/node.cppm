module;

export module dx_wrapper.gltf.node;
import std;
import dx_wrapper.gltf.mesh;
import dx_wrapper.core.transform;
import dx_wrapper.gltf.primitive;

export template <typename T = GltfPrimitive>
	requires std::is_base_of_v<GltfPrimitive, T>
struct GltfNode
{
	Transform m_transform;
	
	std::shared_ptr<GltfNode> m_parent = nullptr;
	
	// Contents
	std::shared_ptr<GltfMesh<T>> m_mesh = nullptr;
};