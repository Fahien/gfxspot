#pragma once

#include <unordered_map>

#include "spot/gfx/descriptors.h"

namespace spot::gfx
{

class ShaderModule;
class Device;
class Swapchain;
class RenderPass;
class Renderer;
struct Resources;
struct Primitive;

class PipelineLayout
{
  public:
	PipelineLayout( Device& d,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings,
		std::optional<VkPushConstantRange> constants = std::nullopt
	);
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
		const VkPipelineColorBlendAttachmentState& color_blend_attachment,
		VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT,
		VkBool32 depth_test = VK_TRUE,
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
