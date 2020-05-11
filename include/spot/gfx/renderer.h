#pragma once

#include <unordered_map>

#include <spot/math/math.h>
#include <spot/gltf/mesh.h>

#include "spot/gfx/buffers.h"
#include "spot/gfx/descriptors.h"
#include "spot/gfx/images.h"
#include "spot/gfx/pipelines.h"


namespace spot::gfx
{

class Node;
struct Primitive;
struct Material;

class Device;
class Swapchain;
class Graphics;


/// @todo Is this not used anymore?
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


/// @brief Vulkan resources for Primitives.
/// Multiple primitives that are actually equal could use the same vertex and index buffer
/// Those primitives may have different materials and belong to different nodes with different transforms
/// That is why mvp-bo and material-bo are stored in their own resource set
struct PrimitiveResources
{
	PrimitiveResources( const Device& device, const Primitive& pm );

	Buffer vertex_buffer;
	Buffer index_buffer;
};


struct DescriptorResources
{
	DescriptorResources(
		const Renderer& renderer,
		const GraphicsPipeline& gp,
		const Handle<Node>& node,
		const Handle<Material>& material = {} );

	uint64_t pipeline;

	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;

	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};


/// @brief Rendering resources for Nodes
struct NodeResources
{
	NodeResources( const Swapchain& swapchain );

	/// MVP uniform buffer objects for each swapchain images
	std::vector<Buffer> ubos;
};


/// @brief Rendering resources for Materials
struct MaterialResources
{
	MaterialResources( const Swapchain& swapchain );

	/// Material uniform buffer objects for each swapchain image
	std::vector<Buffer> ubos;

	/// A sampler for the diffuse texture
	Sampler sampler;
};


/// @brief Rendering resources for Ambient light
struct AmbientResources
{
	AmbientResources( const Swapchain& swapchain );

	/// Ambient uniform buffer objects for each swapchain image
	std::vector<Buffer> ubos;
};


/// @brief Rendering resources for lights
struct LightResources
{
	LightResources( const Swapchain& swapchain );

	/// Ambient uniform buffer objects for each swapchain image
	std::vector<Buffer> ubos;
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

	void add( const Handle<Node>& node );
	void add( const Handle<Node>& node, const Primitive& prim );

	std::unordered_map<size_t, DescriptorResources>::iterator add_descriptors( const Handle<Node>& node, const Handle<Material>& material );

	Graphics& gfx;

	/// @brief Collection of pipelines
	std::vector<GraphicsPipeline> pipelines;

	/// @brief The key is a hash value of the primitive
	/// Meshes with the same primitive will use the same resources
	std::unordered_map<size_t, PrimitiveResources> primitive_resources;

	/// @brief Key is material handle, value is ubos for material
	std::unordered_map<Handle<Material>, MaterialResources> material_resources;

	/// @brief Key is node handle, value is ubos for frames
	std::unordered_map<Handle<Node>, NodeResources> node_resources;

	/// @brief Key is hash of node and material
	/// Value is descriptor sets for this node and material
	std::unordered_map<size_t, DescriptorResources> descriptor_resources;

	AmbientResources ambient_resources;

	/// @brief Key is hash of node and light
	/// The same light may be attached to multiple nodes
	/// Value is ubos for frames
	std::unordered_map<size_t, LightResources> light_resources;

  private:
	/// @return Find the line pipeline with a specific width
	uint64_t find_pipeline( float line_width );
};


} // namespace spot::gfx
