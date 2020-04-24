#include "spot/gfx/renderer.h"

#include <cassert>

#include "spot/gfx/graphics.h"
#include "spot/gfx/models.h"
#include "spot/gfx/hash.h"

#define FIND( container, object ) ( container.find( object ) != std::end( container ) )

namespace gtf = spot::gltf;

namespace spot::gfx
{


template <typename T>
VkVertexInputBindingDescription get_bindings()
{
	VkVertexInputBindingDescription bindings = {};
	bindings.binding = 0;
	bindings.stride = sizeof( Vertex );
	bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindings;
}


template <>
std::vector<VkVertexInputAttributeDescription> get_attributes<Dot>()
{
	std::vector<VkVertexInputAttributeDescription> attributes( 2 );

	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof( Vertex, p );

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributes[1].offset = offsetof( Vertex, c );

	return attributes;
}

template <>
std::vector<VkVertexInputAttributeDescription> get_attributes<Vertex>()
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



Renderer::Renderer( Graphics& g )
: gfx { g }
{
	recreate_pipelines();
}


void Renderer::recreate_pipelines()
{
	pipelines.clear();

	auto mesh_pipeline = GraphicsPipeline(
		get_bindings<Vertex>(),
		get_attributes<Vertex>(),
		gfx.mesh_layout,
		gfx.mesh_vert,
		gfx.mesh_frag,
		gfx.render_pass,
		gfx.viewport.get_viewport(),
		gfx.scissor );
	mesh_pipeline.index = 0;
	pipelines.emplace_back( std::move( mesh_pipeline ) );

	auto mesh_no_image_pipeline = GraphicsPipeline(
		get_bindings<Vertex>(),
		get_attributes<Vertex>(),
		gfx.mesh_no_image_layout,
		gfx.mesh_no_image_vert,
		gfx.mesh_no_image_frag,
		gfx.render_pass,
		gfx.viewport.get_viewport(),
		gfx.scissor );
	mesh_no_image_pipeline.index = 1;
	pipelines.emplace_back( std::move( mesh_no_image_pipeline ) );

	auto line_pipeline = GraphicsPipeline(
		get_bindings<Dot>(),
		get_attributes<Dot>(),
		gfx.line_layout,
		gfx.line_vert,
		gfx.line_frag,
		gfx.render_pass,
		gfx.viewport.get_viewport(),
		gfx.scissor,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
	line_pipeline.index = 2;
	pipelines.emplace_back( std::move( line_pipeline ) );
}


std::vector<VkDescriptorPoolSize> get_uniform_pool_sizes( const uint32_t count )
{
	VkDescriptorPoolSize pool_size = {};
	pool_size.descriptorCount = count;
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	return { pool_size };
}


DynamicResources::DynamicResources( Device& d, Swapchain& s, GraphicsPipeline& gp )
: vertex_buffer { d, sizeof( Dot ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT }
, index_buffer { d, sizeof( Index ), VK_BUFFER_USAGE_INDEX_BUFFER_BIT }
, uniform_buffers {}
, pipeline { gp.index }
, descriptor_pool { d,
	get_uniform_pool_sizes( s.images.size() ),
	uint32_t( s.images.size() ) }
, descriptor_sets { descriptor_pool.allocate( gp.layout.descriptor_set_layout, s.images.size() ) }
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


std::vector<Buffer> create_mvp_ubos( const Swapchain& swapchain )
{
	std::vector<Buffer> ret;

	for ( size_t i = 0; i < swapchain.images.size(); ++i )
	{
		ret.emplace_back(
			Buffer(
				swapchain.device,
				sizeof( UniformBufferObject ),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			)
		);
	}

	return ret;
}


std::vector<Buffer> create_mat_ubos( const Swapchain& swapchain )
{
	std::vector<Buffer> ret;

	for ( size_t i = 0; i < swapchain.images.size(); ++i )
	{
		ret.emplace_back(
			Buffer(
				swapchain.device,
				sizeof( Material::Ubo ),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			)
		);
	}

	return ret;
}


NodeResources::NodeResources( const Swapchain& swapchain )
: ubos { create_mvp_ubos( swapchain ) }
{
}


MaterialResources::MaterialResources( const Swapchain& swapchain )
: ubos { create_mat_ubos( swapchain ) }
, sampler { swapchain.device }
{
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


PrimitiveResources::PrimitiveResources( const Device& device, const Primitive& primitive )
: vertex_buffer {
		device,
		primitive.vertices.size() * sizeof( Vertex ),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT }
, index_buffer {
		device,
		primitive.indices.size() * sizeof( Index ),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT }
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
}


DescriptorResources::DescriptorResources( const Renderer& renderer, const GraphicsPipeline& pipel, const gltf::Handle<gltf::Node> node, const Material* material )
: pipeline { pipel.index }
, descriptor_pool {
		renderer.gfx.swapchain.device,
		get_mesh_pool_size( renderer.gfx.swapchain.images.size() * 2 ),
		uint32_t( renderer.gfx.swapchain.images.size() * 2 )
	}
, descriptor_sets {
		descriptor_pool.allocate(
			pipel.layout.descriptor_set_layout,
			renderer.gfx.swapchain.images.size()
		)
	}
{
	assert( renderer.node_resources.count( node ) && "Node resources were not created" );
	const auto& node_res = renderer.node_resources.at( node );

	for ( size_t i = 0; i < renderer.gfx.swapchain.images.size(); ++i )
	{
		std::vector<VkWriteDescriptorSet> writes = {};

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = node_res.ubos[i].handle;
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
		if ( material )
		{
			assert( renderer.material_resources.count( material->index ) && "Material resources were not created" );
			auto& mat_res = renderer.material_resources.at( material->index );

			mat_info.buffer = mat_res.ubos[i].handle;
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
		if ( material && material->texture != VK_NULL_HANDLE )
		{
			assert( renderer.material_resources.count( material->index ) && "Material resources were not created" );
			auto& mat_res = renderer.material_resources.at( material->index );

			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info.imageView = material->texture;
			image_info.sampler = mat_res.sampler.handle;

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

		vkUpdateDescriptorSets( pipel.device.handle, writes.size(), writes.data(), 0, nullptr );
	}
}


/// @return The index of the standard mesh pipeline, which is at index 0 by default
uint64_t get_mesh_pipeline()
{
	return 0;
}


/// @return The index of the no-image-mesh pipeline, which is at index 1 by default
uint64_t get_mesh_no_image_pipeline()
{
	return 1;
}


/// @return The index of the line pipeline, 2 by default
uint64_t get_line_pipeline()
{
	return 2;
}


/// @return The pipeline to use for this material
uint64_t select_pipeline( Material* material )
{
	if ( material )
	{
		if ( material->texture != VK_NULL_HANDLE )
		{
			return get_mesh_pipeline();
		}

		return get_mesh_no_image_pipeline();
	}

	return get_line_pipeline();
}


void Renderer::add( const gltf::Handle<gltf::Node> node, const Primitive& prim )
{
	// We need vertex and index buffers. These are stored in primitive resources
	auto hash_prim = hash( prim );
	// Avoid duplication of primitive resources
	if ( !FIND( primitive_resources, hash_prim ) )
	{
		primitive_resources.emplace( hash_prim, PrimitiveResources( gfx.device, prim ) );
	}

	add_descriptors( node, prim.get_material() );
}


void Renderer::add( const gltf::Handle<gltf::Node> node )
{
	if ( node->mesh < 0 )
	{
		return; // no mesh to add
	}

	// The node has a mesh, therefore we need UBOs for the MVP matrix
	// MVP ubos are store in node resources
	if ( !FIND( node_resources, node ) )
	{
		node_resources.emplace( node, NodeResources( gfx.swapchain ) );
	}

	// Now get the mesh, and its primitives
	auto mesh = gfx.models.meshes[node->mesh];
	for ( auto& prim : mesh.primitives )
	{
		add( node, prim );
	}
}


std::unordered_map<size_t, DescriptorResources>::iterator
Renderer::add_descriptors( const gltf::Handle<gltf::Node> node, const int32_t material_index )
{
	// We need descriptors for the MVP ubos and material textures
	// A node may have a mesh with multiple primitives with different materials
	// And the same material may appear into multiple primitives of different nodes
	// So we hash combine both node and material
	auto key = hash( node.get_index(), material_index );
	auto it = descriptor_resources.find( key );
	if ( it != std::end( descriptor_resources ) )
	{
		// Skip if we already have resources associated to this pair ( node, material )
		return it;
	}

	auto material = gfx.models.get_material( material_index );
	if ( material )
	{
		// Add ubo for this material
		if ( material_resources.find( material_index ) == std::end( material_resources ) )
		{
			material_resources.emplace( material_index, gfx.swapchain );
		}
	}

	auto pipeline_index = select_pipeline( material );
	auto& graphics_pipeline = pipelines[pipeline_index];
	auto resource = DescriptorResources( *this, graphics_pipeline, node, material );
	bool ok;
	std::tie( it, ok ) = descriptor_resources.emplace( key, std::move( resource ) );
	assert( ok && "Cannot emplace primitive resource" );
	printf( "Descriptor [node %zu, material %d]\n", node.get_index(), material_index );
	return it;
}


} // namespace spot::gfx
