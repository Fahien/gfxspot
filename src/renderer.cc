#include "spot/gfx/renderer.h"

#include <cassert>
#include <spot/gltf/mesh.h>

#include "spot/gfx/graphics.h"

namespace gtf = spot::gltf;

namespace spot::gfx
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


MeshResources::MeshResources( Device& d, Swapchain& swapchain, PipelineLayout& l, const Primitive& primitive )
: vertex_buffer { d, primitive.vertices.size() * sizeof( Vertex ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT }
, index_buffer { d, primitive.indices.size() * sizeof( Index ), VK_BUFFER_USAGE_INDEX_BUFFER_BIT }
, uniform_buffers {}
, material_ubos {}
, sampler { d }
, descriptor_pool { d,
	get_mesh_pool_size( swapchain.images.size() * 2 ),
	uint32_t( swapchain.images.size() * 2 ) }
, descriptor_sets { descriptor_pool.allocate( l.descriptor_set_layout, swapchain.images.size() ) }
{
	// Upload vertices
	{
		auto data = reinterpret_cast<const uint8_t*>( primitive.vertices.data() );
		auto size = primitive.vertices.size() * sizeof( Vertex );
		vertex_buffer.upload( data, size );
	}

	// Upload indices
	{
		auto data = reinterpret_cast<const uint8_t*>( primitive.indices.data() );
		auto size = primitive.indices.size() * sizeof( Index );
		index_buffer.upload( data, size );
	}

	for ( size_t i = 0; i < swapchain.images.size(); ++i )
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
		if ( primitive.material )
		{
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
		}

		VkDescriptorImageInfo image_info = {};
		if ( primitive.material && primitive.material->texture != VK_NULL_HANDLE )
		{
			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info.imageView = primitive.material->texture;
			image_info.sampler = sampler.handle;

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


void Renderer::add( const Line& line )
{
	// Find Vulkan resources associated to this rect
	auto it = line_resources.find( &line );
	if ( it == std::end( line_resources ) )
	{
		auto[new_it, ok] = line_resources.emplace(
			&line,
			Resources( graphics.device, graphics.swapchain, graphics.line_layout )
		);
		if (ok)
		{
			it = new_it;
		}
	}

	// Vertices
	auto& vertex_buffer = it->second.vertex_buffer;
	vertex_buffer.set_count( line.dots.size() );
	vertex_buffer.upload( reinterpret_cast<const uint8_t*>( line.dots.data() ) );

	// Indices
	auto& index_buffer = it->second.index_buffer;
	index_buffer.set_count( line.indices.size() );
	index_buffer.upload( reinterpret_cast<const uint8_t*>( line.indices.data() ) );
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


size_t hash( const gtf::Node& node, const Primitive& primitive )
{
	size_t hash = reinterpret_cast<size_t>( &node );
	hash += reinterpret_cast<size_t>( &primitive );
	return hash;
}


void Renderer::add( const gtf::Node& node )
{
	auto& mesh = graphics.models.meshes[node.mesh_index];

	for ( auto& primitive : mesh.primitives )
	{
		auto key = hash( node, primitive );
		// Find Vulkan resources associated to this mesh
		auto it = mesh_resources.find( key );

		// If not found, create new resources
		if ( it == std::end( mesh_resources ) )
		{
			PipelineLayout* layout = nullptr;
			if ( primitive.material )
			{
				if ( primitive.material->texture != VK_NULL_HANDLE )
				{

					layout = &graphics.mesh_layout;
				}
				else
				{
					layout = &graphics.mesh_no_image_layout;
				}
			}
			else
			{
				layout = &graphics.line_layout;
			}

			auto resource = MeshResources( graphics.device, graphics.swapchain, *layout, primitive );
			auto [it, ok] = mesh_resources.emplace( key, std::move( resource ) );
			assert( ok && "Cannot emplace mesh resource" );
		}

	}
}


} // namespace spot::gfx
 