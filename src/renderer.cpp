#include "graphics/renderer.hpp"

#include <cassert>

#include "graphics/graphics.hpp"


namespace gfx
{


Renderer::Renderer( Graphics& g )
: graphics { g }
{}


std::vector<VkDescriptorPoolSize> get_uniform_pool_sizes( const uint32_t count )
{
	VkDescriptorPoolSize pool_size = {};
	pool_size.descriptorCount = count;
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	return { pool_size };
}


Resources::Resources( Device& d, Swapchain& s, PipelineLayout& l )
: vertex_buffer { d, sizeof( Dot ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT }
, index_buffer { d, sizeof( Index ), VK_BUFFER_USAGE_INDEX_BUFFER_BIT }
, uniform_buffers {}
, descriptor_pool { d,
	get_uniform_pool_sizes( s.images.size() ),
	uint32_t( s.images.size() ) }
, descriptor_sets { descriptor_pool.allocate( l.descriptor_set_layout, s.images.size() ) }
{
	for ( size_t i = 0; i < s.images.size(); ++i )
	{
		uniform_buffers.emplace_back(
			Buffer( d, sizeof( UniformBufferObject ), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT )
		);

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = uniform_buffers[i].handle;
		buffer_info.offset = 0;
		buffer_info.range = sizeof( UniformBufferObject );

		VkWriteDescriptorSet descriptor_write = {};
		descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet = descriptor_sets[i];
		descriptor_write.dstBinding = 0;
		descriptor_write.dstArrayElement = 0;

		descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_write.descriptorCount = 1;

		descriptor_write.pBufferInfo = &buffer_info;

		vkUpdateDescriptorSets(d.handle, 1, &descriptor_write, 0, nullptr);
	}
}


std::vector<VkDescriptorPoolSize> get_mesh_pool_size( const uint32_t count )
{
	std::vector<VkDescriptorPoolSize> pool_sizes(2);

	pool_sizes[0].descriptorCount = count;
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[1].descriptorCount = count;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	return pool_sizes;
}


MeshResources::MeshResources( Device& d, Swapchain& s, PipelineLayout& l, VkImageView image_view, GraphicsPipeline& p )
: pipeline { p }
, uniform_buffers {}
, material_ubos {}
, sampler { d }
, descriptor_pool { d,
	get_mesh_pool_size( s.images.size() * 2 ),
	uint32_t( s.images.size() * 2 ) }
, descriptor_sets { descriptor_pool.allocate( l.descriptor_set_layout, s.images.size() ) }
{
	for ( size_t i = 0; i < s.images.size(); ++i )
	{
		uniform_buffers.emplace_back(
			Buffer( d, sizeof( UniformBufferObject ), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT )
		);

		material_ubos.emplace_back(
			Buffer( d, sizeof( Material::Ubo ), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT )
		);

		std::vector<VkWriteDescriptorSet> writes = {};

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = uniform_buffers[i].handle;
		buffer_info.offset = 0;
		buffer_info.range = sizeof( UniformBufferObject );
	
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[i];
		write.dstBinding = 0;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &buffer_info;

		writes.emplace_back( write );

		VkDescriptorBufferInfo mat_info = {};
		mat_info.buffer = material_ubos[i].handle;
		mat_info.offset = 0;
		mat_info.range = sizeof( Material::Ubo );
	
		VkWriteDescriptorSet mat_write = {};
		mat_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		mat_write.dstSet = descriptor_sets[i];
		mat_write.dstBinding = 1;
		mat_write.dstArrayElement = 0;
		mat_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mat_write.descriptorCount = 1;
		mat_write.pBufferInfo = &mat_info;

		writes.emplace_back( mat_write );

		VkDescriptorImageInfo image_info = {};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = image_view;
		image_info.sampler = sampler.handle;
		if ( image_view != VK_NULL_HANDLE )
		{
			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = descriptor_sets[i];
			write.dstBinding = 2;
			write.dstArrayElement = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &image_info;

			writes.emplace_back( write );
		}

		vkUpdateDescriptorSets( d.handle, writes.size(), writes.data(), 0, nullptr );
	}
}


void Renderer::add( const Triangle& rect )
{
	// Find Vulkan resources associated to this rect
	auto it = triangle_resources.find( &rect );
	if ( it == std::end( triangle_resources ) )
	{
		auto[new_it, ok] = triangle_resources.emplace(
			&rect,
			Resources( graphics.device, graphics.swapchain, graphics.line_layout )
		);
		if (ok)
		{
			it = new_it;
		}
	}

	// Vertices
	auto& vertex_buffer = it->second.vertex_buffer;
	vertex_buffer.set_count( rect.dots.size() );
	vertex_buffer.upload( reinterpret_cast<const uint8_t*>( rect.dots.data() ) );

	// Indices
	auto& index_buffer = it->second.index_buffer;
	index_buffer.set_count( rect.indices.size() );
	index_buffer.upload( reinterpret_cast<const uint8_t*>( rect.indices.data() ) );
}


void Renderer::add( const Rect& rect )
{
	// Find Vulkan resources associated to this rect
	auto it = rect_resources.find( &rect );
	if ( it == std::end( rect_resources ) )
	{
		auto[new_it, ok] = rect_resources.emplace(
			&rect,
			Resources( graphics.device, graphics.swapchain, graphics.line_layout )
		);
		if (ok)
		{
			it = new_it;
		}
	}

	// Vertices
	auto& vertex_buffer = it->second.vertex_buffer;
	vertex_buffer.set_count( rect.dots.size() );
	vertex_buffer.upload( reinterpret_cast<const uint8_t*>( rect.dots.data() ) );

	// Indices
	auto& index_buffer = it->second.index_buffer;
	index_buffer.set_count( rect.indices.size() );
	index_buffer.upload( reinterpret_cast<const uint8_t*>( rect.indices.data() ) );
}


VkVertexInputBindingDescription get_bindings( const Primitive& p )
{
	VkVertexInputBindingDescription bindings = {};
	bindings.binding = 0;
	bindings.stride = p.stride;
	bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindings;
}

std::vector<VkVertexInputAttributeDescription> get_attributes( const Primitive& primitive )
{
	std::vector<VkVertexInputAttributeDescription> attributes( 4 );

	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof( Vertex, p );

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof( Vertex, n );

	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributes[2].offset = offsetof( Vertex, c );

	attributes[3].binding = 0;
	attributes[3].location = 3;
	attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[3].offset = offsetof( Vertex, t );

	return attributes;
}


void Renderer::add( const Mesh& mesh )
{
	for( auto& primitive : mesh.primitives )
	{
		// Find Vulkan resources associated to this mesh
		auto it = mesh_resources.find( &primitive );

		// If not found, create new resources
		if ( it == std::end( mesh_resources ) )
		{
			auto& layout = primitive.material->texture != VK_NULL_HANDLE ? graphics.mesh_layout : graphics.mesh_no_image_layout;
			auto& vert = primitive.material->texture != VK_NULL_HANDLE ? graphics.mesh_vert : graphics.mesh_no_image_vert;
			auto& frag = primitive.material->texture != VK_NULL_HANDLE ? graphics.mesh_frag : graphics.mesh_no_image_frag;

			/// @todo Creating a pipeline just for this primitive is overkill
			/// Must opt for a caching methodology
			auto pipeline = GraphicsPipeline(
				get_bindings( primitive ),
				get_attributes( primitive ),
				layout,
				vert,
				frag,
				graphics.render_pass,
				graphics.viewport,
				graphics.scissor
			);

			auto [pipe_it, pipe_ok] = pipelines.emplace( &primitive, std::move( pipeline ) );
			assert( pipe_ok && "Cannot emplace pipeline" );

			auto resource = MeshResources( graphics.device, graphics.swapchain, layout, primitive.material->texture, pipe_it->second );
			auto [it, ok] = mesh_resources.emplace( &primitive, std::move( resource ) );
			assert( ok && "Cannot emplace mesh resource" );
		}
	}
}


} // namespace gfx
