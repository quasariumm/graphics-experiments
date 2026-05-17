module;

export module dx_meshlets.gltf.node;
import std;
import dx_meshlets.gltf.mesh;
import dx_wrapper.core.transform;

export struct GltfNode
{
	Transform m_transform;
	
	std::shared_ptr<GltfNode> m_parent = nullptr;
	
	// Contents
	std::shared_ptr<GltfMesh> m_mesh = nullptr;
};