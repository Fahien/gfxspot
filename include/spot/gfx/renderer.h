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

	GraphicsPipeline& pipeline;
	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;

};


struct Resources
{
	Resources( Device& dv, Swapchain& sc, GraphicsPipeline& gp, const Primitive& pm );

	Buffer vertex_buffer;
	Buffer index_buffer;

	// Uniform buffer for each swapchain image
	std::vector<Buffer> uniform_buffers;
	std::vector<Buffer> material_ubos;

	//ImageView image_view;
	Sampler sampler;

	GraphicsPipeline& pipeline;
	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};


class Renderer
{
  public:
	Renderer( Graphics& gfx );

	void add( const Line& ln );
	void add( const Rect& rt );
	std::unordered_map<size_t, Resources>::iterator add( Primitive& pm );

	Graphics& graphics;

	/// @brief Each model will have
	/// - vertex buffer containing constant data about its vertices
	/// - uniform buffers that can change per swapchain image
	/// - DescriptorPool and DescriptorSet per swapchain image
	std::unordered_map<const Line*, DynamicResources> line_resources;
	std::unordered_map<const Rect*, DynamicResources> rect_resources;

	/// @brief The key is a hash value of the primitive
	/// Meshes with the same primitive will use the same resources
	std::unordered_map<size_t, Resources> resources;
};


} // namespace spot::gfx
