module;

#include <meshoptimizer.h>

export module dx_meshlets.external.meshopt_meshlets;
import std;

export namespace Meshopt
{
using Meshlet = ::meshopt_Meshlet;

__forceinline size_t BuildMeshletsBound(size_t index_count, size_t max_vertices, size_t max_triangles)
{ return meshopt_buildMeshletsBound(index_count, max_vertices, max_triangles); }

__forceinline size_t BuildMeshlets(Meshlet* meshlets, std::uint32_t* meshlet_vertices, std::uint8_t* meshlet_triangles,
								   const std::uint32_t* indices, size_t index_count, const float* vertex_positions,
								   size_t vertex_count, size_t vertex_positions_stride, size_t max_vertices,
								   size_t max_triangles, float cone_weight)
{
	return meshopt_buildMeshlets(meshlets,
								 meshlet_vertices,
								 meshlet_triangles,
								 indices,
								 index_count,
								 vertex_positions,
								 vertex_count,
								 vertex_positions_stride,
								 max_vertices,
								 max_triangles,
								 cone_weight);
}

__forceinline size_t BuildMeshletsFlex(Meshlet* meshlets, std::uint32_t* meshlet_vertices, std::uint8_t* meshlet_triangles,
									   const std::uint32_t* indices, size_t index_count, const float* vertex_positions,
									   size_t vertex_count, size_t vertex_positions_stride, size_t max_vertices,
									   size_t min_triangles, size_t max_triangles, float cone_weight, float split_factor)
{
	return meshopt_buildMeshletsFlex(meshlets,
									 meshlet_vertices,
									 meshlet_triangles,
									 indices,
									 index_count,
									 vertex_positions,
									 vertex_count,
									 vertex_positions_stride,
									 max_vertices,
									 min_triangles,
									 max_triangles,
									 cone_weight,
									 split_factor);
}

__forceinline size_t BuildMeshletsScan(Meshlet* meshlets, std::uint32_t* meshlet_vertices, std::uint8_t* meshlet_triangles,
									   const std::uint32_t* indices, size_t index_count, size_t vertex_count,
									   size_t max_vertices, size_t max_triangles)
{
	return meshopt_buildMeshletsScan(meshlets,
									 meshlet_vertices,
									 meshlet_triangles,
									 indices,
									 index_count,
									 vertex_count,
									 max_vertices,
									 max_triangles);
}

__forceinline size_t BuildMeshletsSpatial(Meshlet* meshlets, std::uint32_t* meshlet_vertices, std::uint8_t* meshlet_triangles,
										  const std::uint32_t* indices, size_t index_count, const float* vertex_positions,
										  size_t vertex_count, size_t vertex_positions_stride, size_t max_vertices,
										  size_t min_triangles, size_t max_triangles, float fill_weight)
{
	return meshopt_buildMeshletsSpatial(meshlets,
										meshlet_vertices,
										meshlet_triangles,
										indices,
										index_count,
										vertex_positions,
										vertex_count,
										vertex_positions_stride,
										max_vertices,
										min_triangles,
										max_triangles,
										fill_weight);
}

__forceinline void OptimizeMeshlet(std::uint32_t* meshlet_vertices, std::uint8_t* meshlet_triangles, size_t triangle_count,
								   size_t vertex_count)
{ meshopt_optimizeMeshlet(meshlet_vertices, meshlet_triangles, triangle_count, vertex_count); }
} // namespace Meshopt
