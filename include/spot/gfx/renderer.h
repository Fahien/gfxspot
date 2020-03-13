#pragma once

#include <unordered_map>

#include <spot/math/math.h>

#include "spot/gfx/buffers.h"
#include "spot/gfx/descriptors.h"
#include "spot/gfx/images.h"
#include "spot/gfx/pipelines.h"

#include "spot/gltf/node.h"

namespace spot::gfx
{

struct Line;
struct Rect;
struct Mesh;
struct Primitive;
struct Material;

class Device;
class Swapchain;
class Graphics;
class Models;


struct DynamicResources
{
	DynamicResources( Device& d, Swapchain& s, GraphicsPipeline& pl );

	// Vertices and indices do not change, hence one is enough
	DynamicBuffer vertex_buffer;
	DynamicBuffer index_buffer;

	// Uniform buffer for each swapchain image
	std::vector<Buffer> uniform_buffers;

	uint64_t pipeline = 0;
	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;

};


/// @todo This resources associate too many things
/// To improve this, consider that:
/// - Primitives that are equal should have same vertex and index buffer
/// - Those primitives may have different materials and different uniform
/// That is why mvp-bo and material-bo should each be stored in their own resource set
/// @ref Renderer
struct PrimitiveResources
{
	PrimitiveResources( const Renderer& renderer, const Primitive& pm );

	Buffer vertex_buffer;
	Buffer index_buffer;
};


struct DescriptorResources
{
	DescriptorResources(
		const Renderer& renderer,
		const GraphicsPipeline& gp,
		uint64_t node,
		const Material* material = nullptr );

	uint64_t pipeline;
	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};


struct NodeResources
{
	NodeResources( const Swapchain& swapchain );

	std::vector<Buffer> ubos;
};

struct MaterialResources
{
	MaterialResources( const Swapchain& swapchain );

	std::vector<Buffer> ubos;
	Sampler sampler;
};

/// @todo Resource cache should be improved in this way:
/// 1. A resource cache for primitives
/// 2. A resource cache for mvp-bos
/// 3. A resource cache for mat-bos
/// Then, a descriptor set cache will use as indices an hash combine of
/// both the nodei (for the mvp-bo) and the materiali (for the mat-bo)
/// Note: An helper method can be created to get a descriptor pool from
///       a pipeline layout, so the pipeline layout will store these pools.
class Renderer
{
  public:
	Renderer( Graphics& gfx );

	void recreate_pipelines();

	void add( const Line& ln );
	void add( const Rect& rt );
	void add( uint32_t node );

	Graphics& graphics;

	/// @brief Collection of pipelines
	std::vector<GraphicsPipeline> pipelines;

	/// @brief Each model will have
	/// - vertex buffer containing constant data about its vertices
	/// - uniform buffers that can change per swapchain image
	/// - DescriptorPool and DescriptorSet per swapchain image
	std::unordered_map<const Line*, DynamicResources> line_resources;
	std::unordered_map<const Rect*, DynamicResources> rect_resources;

	/// @brief The key is a hash value of the primitive
	/// Meshes with the same primitive will use the same resources
	std::unordered_map<size_t, PrimitiveResources> primitive_resources;

	/// @brief Key is node index
	/// Value is ubos for frames
	std::unordered_map<uint32_t, NodeResources> node_resources;

	/// @brief Key is material index
	/// Value is ubos for material
	std::unordered_map<uint32_t, MaterialResources> material_resources;

	/// @brief Key is hash of node and material
	/// Value is descriptor sets for this node and material
	std::unordered_map<size_t, DescriptorResources> descriptor_resources;

  private:
	/// @return Find the line pipeline with a specific width
	uint64_t find_pipeline( float line_width );
};


} // namespace spot::gfx
