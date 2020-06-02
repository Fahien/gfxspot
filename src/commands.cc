#include "spot/gfx/commands.h"

#include <cassert>

#include "spot/gfx/graphics.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{


CommandBuffer::CommandBuffer( const VkCommandBuffer h )
: handle { h }
{}


void CommandBuffer::begin( const VkCommandBufferUsageFlags usage_flags )
{
	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = usage_flags;

	auto ret = vkBeginCommandBuffer( handle, &info );
	assert( ret == VK_SUCCESS && "Cannot begin command buffer" );
}

VkImageAspectFlags get_aspect_mask( const VkImageLayout layout )
{
	if ( layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	return VK_IMAGE_ASPECT_COLOR_BIT;
}

void CommandBuffer::transition( VulkanImage& image, const VkImageLayout layout )
{
	VkImageMemoryBarrier barrier = {};

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = image.layout;
	barrier.newLayout = layout;

	barrier.image = image.vkhandle;
	barrier.subresourceRange.aspectMask = get_aspect_mask( layout );
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags src_stage = 0;
	VkPipelineStageFlags dst_stage = 0;

	if ( image.layout == VK_IMAGE_LAYOUT_UNDEFINED &&
		layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ( image.layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if ( image.layout == VK_IMAGE_LAYOUT_UNDEFINED &&
		layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if ( image.layout == VK_IMAGE_LAYOUT_UNDEFINED &&
		layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		assert( false && "Transition not supported" );
	}

	vkCmdPipelineBarrier( handle, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier );

	image.layout = layout;
}


void CommandBuffer::copy( const Buffer& from_buffer, const VulkanImage& dest_image )
{
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = dest_image.extent;

	vkCmdCopyBufferToImage( handle, from_buffer.handle, dest_image.vkhandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );
}


void CommandBuffer::image_memory_barrier( const VkImageMemoryBarrier& barrier, const VkPipelineStageFlags src, const VkPipelineStageFlags dst )
{
	vkCmdPipelineBarrier( handle, src, dst, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


void CommandBuffer::begin_render_pass( RenderPass& render_pass, Framebuffer& framebuffer )
{
	VkRenderPassBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass = render_pass.handle;
	info.framebuffer = framebuffer.handle;
	info.renderArea.offset = { 0, 0 };
	info.renderArea.extent = framebuffer.extent;

	std::array<VkClearValue, 2> clears;
	clears[0].color = { .2f, .2f, .2f, 1.0f };
	clears[1].depthStencil = { 1.0f, 0 };
	info.clearValueCount = clears.size();
	info.pClearValues = clears.data();

	vkCmdBeginRenderPass( handle, &info, VK_SUBPASS_CONTENTS_INLINE );
}


void CommandBuffer::bind( GraphicsPipeline& line_pipeline )
{
	vkCmdBindPipeline( handle, VK_PIPELINE_BIND_POINT_GRAPHICS, line_pipeline.handle );
}


void CommandBuffer::set_line_width( const float line_width )
{
	vkCmdSetLineWidth( handle, line_width );
}


void CommandBuffer::set_viewport( const VkViewport& viewport )
{
	vkCmdSetViewport( handle, 0, 1, &viewport );
}


void CommandBuffer::set_scissor( const VkRect2D& scissor )
{
	vkCmdSetScissor( handle, 0, 1, &scissor );
}


void CommandBuffer::push_constants( const PipelineLayout& layout, VkShaderStageFlags stages, uint32_t offset, uint32_t size, void* bytes )
{
	assert( size && "Cannot push 0 constants" );
	assert( bytes && "Not constant memory to push" );
	vkCmdPushConstants( handle, layout.handle, stages, offset, size, bytes );
}


void CommandBuffer::bind_vertex_buffer( Buffer& buffer, VkDeviceSize offset )
{
	VkDeviceSize offsets[] = { offset };
	vkCmdBindVertexBuffers( handle, 0, 1, &buffer.handle, offsets );
}


void CommandBuffer::bind_vertex_buffer( DynamicBuffer& buffer )
{
	vkCmdBindVertexBuffers( handle, 0, 1, &buffer.handle, &buffer.offset );
}


void CommandBuffer::bind_index_buffer( Buffer& buffer, VkDeviceSize offset )
{
	vkCmdBindIndexBuffer( handle, buffer.handle, offset, VK_INDEX_TYPE_UINT16 );
}


void CommandBuffer::bind_index_buffer( DynamicBuffer& buffer )
{
	vkCmdBindIndexBuffer( handle, buffer.handle, 0, VK_INDEX_TYPE_UINT16 );
}


void CommandBuffer::bind_descriptor_sets( const PipelineLayout& layout, const VkDescriptorSet set )
{
	vkCmdBindDescriptorSets( handle, VK_PIPELINE_BIND_POINT_GRAPHICS, layout.handle, 0, 1, &set, 0, nullptr );
}


void CommandBuffer::draw( const uint32_t vertex_count )
{
	assert( vertex_count > 0 && "Cannot draw 0 vertices" );
	vkCmdDraw( handle, vertex_count, 1, 0, 0 );
}


void CommandBuffer::draw_indexed( const uint32_t index_count )
{
	assert( index_count > 0 && "Cannot draw 0 indices" );
	vkCmdDrawIndexed( handle, index_count, 1, 0, 0, 0 );
}


void CommandBuffer::draw_indexed(
		const uint32_t index_count,
		const uint32_t instance_count,
		const uint32_t index_offset,
		const uint32_t vertex_offset )
{
	assert( index_count > 0 && "Cannot draw 0 indices" );
	assert( instance_count > 0 && "Cannot draw 0 instances" );
	vkCmdDrawIndexed( handle, index_count, instance_count, index_offset, vertex_offset, 0 );
}


void CommandBuffer::end_render_pass()
{
	vkCmdEndRenderPass( handle );
}


void CommandBuffer::end()
{
	auto ret = vkEndCommandBuffer( handle );
	assert( ret == VK_SUCCESS && "Cannot end command buffer" );
}


CommandPool::CommandPool( Device& d, uint32_t family_index )
: device { d }
{
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.queueFamilyIndex = family_index;
	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	auto ret = vkCreateCommandPool( device.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create command pool" );
}


CommandPool::~CommandPool()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyCommandPool( device.handle, handle, nullptr );
	}
}


CommandPool::CommandPool( CommandPool&& other )
: device { other.device }
, handle { other.handle }
{
	other.handle = VK_NULL_HANDLE;
}


CommandPool& CommandPool::operator=( CommandPool&& other )
{
	assert( device.handle == other.device.handle && "Cannot move command pool from different devices" );
	std::swap( handle, other.handle );

	return *this;
}


std::vector<CommandBuffer> CommandPool::allocate_command_buffers( const uint32_t count )
{
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandPool = handle;
	info.commandBufferCount = count;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	std::vector<VkCommandBuffer> handles( count );
	auto ret = vkAllocateCommandBuffers( device.handle, &info, handles.data() );
	assert( ret == VK_SUCCESS && "Cannot allocate command buffers" );

	std::vector<CommandBuffer> buffers( count );
	std::transform( std::begin( handles ), std::end( handles ), std::begin( buffers ),
		[]( auto handle ) { return handle; }
	);

	return buffers;
}


} // namespace spot::gfx
