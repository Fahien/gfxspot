#include "spot/gfx/gui_resources.h"

#include <cassert>

#include "spot/gfx/device.h"
#include "spot/gfx/gui.h"
#include "spot/gfx/pipelines.h"


namespace spot::gfx
{


std::vector<VkVertexInputBindingDescription> get_gui_bindings()
{
	// Vertex input state
	VkVertexInputBindingDescription bindings = {};
	bindings.stride = sizeof( ImDrawVert );

	return { bindings };
}


std::vector<VkVertexInputAttributeDescription> get_gui_attributes()
{
	// Location 0: Position
	VkVertexInputAttributeDescription pos_attr = {};
	pos_attr.format = VK_FORMAT_R32G32_SFLOAT;
	pos_attr.offset = offsetof( ImDrawVert, pos );

	// Location 1: UV
	VkVertexInputAttributeDescription uv_attr = {};
	uv_attr.location = 1;
	uv_attr.format = VK_FORMAT_R32G32_SFLOAT;
	uv_attr.offset = offsetof( ImDrawVert, uv );

	// Location 2: Color
	VkVertexInputAttributeDescription col_attr = {};
	col_attr.location = 2;
	col_attr.format = VK_FORMAT_R8G8B8A8_UNORM;
	col_attr.offset = offsetof( ImDrawVert, col );

	return { pos_attr, uv_attr, col_attr };
}


std::vector<VkDescriptorPoolSize> get_gui_pool_size( const uint32_t count )
{
	std::vector<VkDescriptorPoolSize> pool_sizes( 1 );

	pool_sizes[0].descriptorCount = count;
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	return pool_sizes;
}


GuiResources::GuiResources( const GraphicsPipeline& pipel, const Gui& gui, const uint32_t swapchain_count )
    : pipeline { pipel.index }
    , descriptor_pool { pipel.device, get_gui_pool_size( swapchain_count ), swapchain_count }
    , descriptor_sets { descriptor_pool.allocate( pipel.layout.descriptor_set_layout, swapchain_count ) }
{
	assert( swapchain_count > 0 && "Swapchain images should be more than 0" );

	for ( size_t i = 0; i < swapchain_count; ++i )
	{
		vertex_buffers.emplace_back(
		    DynamicBuffer( pipel.device, sizeof( ImDrawVert ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ) );

		index_buffers.emplace_back( DynamicBuffer( pipel.device, sizeof( ImDrawIdx ), VK_BUFFER_USAGE_INDEX_BUFFER_BIT ) );

		VkDescriptorImageInfo image_info = {};

		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = gui.font_view->vkhandle;
		image_info.sampler = gui.sampler.handle;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[i];
		write.dstBinding = 0;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &image_info;

		std::vector<VkWriteDescriptorSet> writes = { write };
		vkUpdateDescriptorSets( pipel.device.handle, writes.size(), writes.data(), 0, nullptr );
	}
}


void Gui::draw( CommandBuffer& command_buffer )
{
	// Render commands
	ImDrawData* draw_data = ImGui::GetDrawData();
	int32_t     vertex_offset = 0;
	uint32_t    index_offset = 0;

	if ( draw_data->CmdListsCount > 0 )
	{
		for ( int32_t i = 0; i < draw_data->CmdListsCount; i++ )
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[i];

			for ( int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++ )
			{
				const ImDrawCmd* cmd = &cmd_list->CmdBuffer[j];

				VkRect2D         scissor_rect;

				scissor_rect.offset.x = std::max( static_cast<int32_t>( cmd->ClipRect.x ), 0 );
				scissor_rect.offset.y = std::max( static_cast<int32_t>( cmd->ClipRect.y ), 0 );
				scissor_rect.extent.width = static_cast<uint32_t>( cmd->ClipRect.z - cmd->ClipRect.x );
				scissor_rect.extent.height = static_cast<uint32_t>( cmd->ClipRect.w - cmd->ClipRect.y );

				// Consider window scale
				float scale = 1;
				scissor_rect.offset.x *= scale;
				scissor_rect.offset.y *= scale;
				scissor_rect.extent.width *= scale;
				scissor_rect.extent.height *= scale;

				command_buffer.set_scissor( scissor_rect );
				command_buffer.draw_indexed( cmd->ElemCount, 1, index_offset, vertex_offset );
				index_offset += cmd->ElemCount;
			}

			vertex_offset += cmd_list->VtxBuffer.Size;
		}
	}
}

}  // namespace spot::gfx