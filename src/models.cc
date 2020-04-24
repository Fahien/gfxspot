#include "spot/gfx/models.h"

#include <cassert>
#include <spot/gltf/gltf.h>

#include "spot/gfx/graphics.h"


namespace spot::gfx
{

const Color Color::black = { 0.0f, 0.0f, 0.0f };
const Color Color::white = { 1.0f, 1.0f, 1.0f };
const Color Color::red = { 1.0f, 0.0f, 0.0f };
const Color Color::green = { 0.0f, 1.0f, 0.0f };
const Color Color::blue = { 0.0f, 0.0f, 1.0f };
const Color Color::yellow = { 1.0f, 1.0f, 0.0f };


Models::Models( Graphics& g )
: graphics { g }
, images{ g.device }
{}


/// @todo Implement
Handle<Node> Models::create_text( const std::string& text )
{
	auto group = gltf.create_node();
	// For each character in the text
	for ( auto c : text )
	{
		// We need to create a node
		auto node = gltf.create_node( group );
		// Its mesh will be a quad
		// Quad's material will be the same for each character
		// Material will have a texture to the bitmap font
		// Only texture coordinates will change between each different character
	}
	return group;
}


Scene& Models::load( const std::string& path )
{
	gltf = Gltf::load( path );

	// Load materials
	for ( auto& material : *gltf.materials )
	{
		if ( material.texture_handle )
		{
			auto& source = material.texture_handle->source;
			assert( source && "Texture has no source" );
			material.texture = images.load( source->uri.c_str() );
		}
	}

	// A primitive without material does not exist in gltf
	// Therefore we a white material at the endMaterial white {
	
	auto white = gltf.create_material( Color::white );

	// Load meshes
	for ( auto& m : *gltf.meshes )
	{
		for ( auto& p : m.primitives )
		{
			// Check valid material
			if ( !p.material )
			{
				p.material = white;
			}

			std::vector<Index> indices;

			// Indices
			if ( auto& accessor = p.indices_handle )
			{
				indices.resize( accessor->count );
				std::memcpy( indices.data(), accessor->get_data(), accessor->get_size() );

				assert( accessor->component_type == Accessor::ComponentType::UNSIGNED_SHORT );
				assert( accessor->type == Accessor::Type::SCALAR );
			}

			// Vertex attributes
			std::vector<Vertex> vertices;

			for ( auto [semantic, accessor] : p.attributes )
			{
				auto data = accessor->get_data();
				auto elem_size = accessor->get_size() / accessor->count;
				auto stride = accessor->get_stride();

				if ( vertices.empty() )
				{
					vertices.resize( accessor->count );
				}

				switch ( semantic )
				{
				case Primitive::Semantic::POSITION:
				{
					if ( stride == 0 )
					{
						stride = sizeof( math::Vec3 );
					}
				
					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.p.x, vert_data, elem_size );
					}

					assert( accessor->component_type == Accessor::ComponentType::FLOAT );
					assert( accessor->type == Accessor::Type::VEC3 );
					break;
				}
				case Primitive::Semantic::NORMAL:
				{
					if ( stride == 0 )
					{
						stride = sizeof( math::Vec3 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.n.x, vert_data, elem_size );
					}

					assert( accessor->component_type == Accessor::ComponentType::FLOAT );
					assert( accessor->type == Accessor::Type::VEC3 );
					break;
				}
				case Primitive::Semantic::TEXCOORD_0:
				{
					if ( stride == 0 )
					{
						stride = sizeof( math::Vec2 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.t.x, vert_data, elem_size );
					}

					assert( accessor->component_type == Accessor::ComponentType::FLOAT );
					assert( accessor->type == Accessor::Type::VEC2 );
					break;
				}
				case Primitive::Semantic::COLOR_0:
				{
					if ( stride == 0 )
					{
						stride = sizeof( Color );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.c.r, vert_data, elem_size );
					}

					assert( accessor->component_type == Accessor::ComponentType::FLOAT );
					assert( accessor->type == Accessor::Type::VEC4 );
					break;
				}
				default:
				{
					assert( false && "Semantic not supported" );
				}
				}
			}

			p.vertices = vertices;
			p.indices = indices;
		}
	}

	return *gltf.scene;
}


} // namespace spot::gfx
