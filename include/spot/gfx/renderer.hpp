#pragma once

#include <unordered_map>

#include "spot/gfx/buffers.h"
#include "spot/gfx/descriptors.h"
#include "spot/gfx/images.h"
#include "spot/gfx/pipelines.h"

namespace spot::gfx
{

struct Rect;
struct Triangle;
struct Mesh;
struct Primitive;

class Device;
class Swapchain;
class Graphics;

struct Resources
{
	Resources( Device& d, Swapchain& s, PipelineLayout& l );

	// Vertices and indices do not change, hence one is enough
	DynamicBuffer vertex_buffer;
	DynamicBuffer index_buffer;

	// Uniform buffer for each swapchain image
	std::vector<Buffer> uniform_buffers;

	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};


struct MeshResources
{
	MeshResources( Device& dv, Swapchain& sc, PipelineLayout& pl, const Primitive& prim );

	Buffer vertex_buffer;
	Buffer index_buffer;

	// Uniform buffer for each swapchain image
	std::vector<Buffer> uniform_buffers;
	std::vector<Buffer> material_ubos;

	//ImageView image_view;
	Sampler sampler;

	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};

class Renderer
{
  public:
	Renderer( Graphics& g );

	void add( const Triangle& t );
	void add( const Rect& r );
	void add( const Mesh& m );

	Graphics& graphics;

	/// @brief Each model will have
	/// - vertex buffer containing constant data about its vertices
	/// - uniform buffers that can change per swapchain image
	/// - DescriptorPool and DescriptorSet per swapchain image
	std::unordered_map<const Rect*, Resources> rect_resources;
	std::unordered_map<const Triangle*, Resources> triangle_resources;
	std::unordered_map<const Primitive*, MeshResources> mesh_resources;
	std::unordered_map<const Primitive*, GraphicsPipeline> pipelines;
};

}
