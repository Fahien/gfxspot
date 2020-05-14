#pragma once

#include <vector>
#include <imgui.h>

#include "spot/gfx/buffers.h"
#include "spot/gfx/descriptors.h"

namespace spot::gfx
{

VkVertexInputBindingDescription get_gui_bindings();
std::vector<VkVertexInputAttributeDescription> get_gui_attributes();

class GraphicsPipeline;
class Gui;

std::pair<std::vector<ImDrawVert>, std::vector<ImDrawIdx>> gui_draw();

struct GuiResources
{
	GuiResources( const GraphicsPipeline& gp, const Gui& gui, const uint32_t swapchain_count );

	// Gui vertices and indices may change between frames
	// We need one for each swapchain image
	std::vector<DynamicBuffer> vertex_buffers;
	std::vector<DynamicBuffer> index_buffers;

	uint64_t pipeline = 0;

	/// Descriptor pool for descriptor sets
	DescriptorPool descriptor_pool;
	// Descriptor sets for each swapchain image
	std::vector<VkDescriptorSet> descriptor_sets;
};

}  // namespace spot::gfx