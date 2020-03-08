#pragma once

#include "spot/gfx/descriptors.h"

namespace spot::gfx
{

class ShaderModule;
class Device;
class RenderPass;


class PipelineLayout
{
  public:
	PipelineLayout( Device& d, const std::vector<VkDescriptorSetLayoutBinding>& bindings );
	~PipelineLayout();

	Device& device;
	DescriptorSetLayout descriptor_set_layout;

	VkPipelineLayout handle = VK_NULL_HANDLE;
};


class GraphicsPipeline
{
  public:
	GraphicsPipeline(
		VkVertexInputBindingDescription bindings,
		const std::vector<VkVertexInputAttributeDescription>& attributes,
		PipelineLayout& layout,
		ShaderModule& vert,
		ShaderModule& frag,
		RenderPass& render_pass,
		const VkViewport& viewport,
		const VkRect2D& scissor,
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
	~GraphicsPipeline();

	GraphicsPipeline( GraphicsPipeline&& o );
	GraphicsPipeline& operator=( GraphicsPipeline&& o );

	Device& device;
	PipelineLayout& layout;

	VkPipeline handle = VK_NULL_HANDLE;

	/// Index used to store the pipeline in a vector
	uint64_t index = 0;
};


} // namespace spot::gfx
