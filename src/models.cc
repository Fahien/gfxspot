#include "graphics/models.h"

#include <cassert>
#include <spot/gltf/gltf.h>

#include "graphics/graphics.hpp"


namespace gfx
{

Models::Models( Graphics& g )
: graphics { g }
, images{ g.device }
{}


void Models::load( const std::string& path )
{
	auto model = spot::gltf::Gltf::load( path );

	// Load materials
	for ( auto& m : model.materials )
	{
		gfx::Material material;

		material.ubo.color.r = m.pbr.base_color_factor[0];
		material.ubo.color.g = m.pbr.base_color_factor[1];
		material.ubo.color.b = m.pbr.base_color_factor[2];
		material.ubo.color.a = m.pbr.base_color_factor[3];

		material.ubo.metallic = m.pbr.metallic_factor;
		material.ubo.roughness = m.pbr.roughness_factor;

		if ( auto texture = m.get_texture() )
		{
			material.texture = images.load( texture->get_source()->uri.c_str() );
		}

		materials.emplace_back( std::move( material ) );
	}

	// Load meshes
	for ( auto& m : model.meshes )
	{
		gfx::Mesh mesh;

		for ( auto& p : m.primitives )
		{
			gfx::Primitive primitive;

			// Material
			if ( p.material_index >= 0 )
			{
				primitive.material = &materials[p.material_index];
			}

			// Indices
			if ( auto accessor = p.get_indices() )
			{
				primitive.indices.resize( accessor->count );
				std::memcpy( primitive.indices.data(), accessor->get_data(), accessor->get_size() );

				assert( accessor->component_type == spot::gltf::Accessor::ComponentType::UNSIGNED_SHORT );
				assert( accessor->type == spot::gltf::Accessor::Type::SCALAR );
			}

			// Vertex attributes
			auto attributes = p.get_attributes();
			for ( auto [semantic, accessor] : attributes )
			{
				auto data = accessor->get_data();
				auto elem_size = accessor->get_size() / accessor->count;
				auto stride = accessor->get_stride();

				if ( primitive.vertices.empty() )
				{
					primitive.vertices.resize( accessor->count );
				}

				switch ( semantic )
				{
				case spot::gltf::Mesh::Primitive::Semantic::POSITION:
				{
					if ( stride == 0 )
					{
						stride = 16;//sizeof( Vec3 );
					}
				
					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = primitive.vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.p.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC3 );
					break;
				}
				case spot::gltf::Mesh::Primitive::Semantic::NORMAL:
				{
					if ( stride == 0 )
					{
						stride = 16;// sizeof( Vec3 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = primitive.vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.n.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC3 );
					break;
				}
				case spot::gltf::Mesh::Primitive::Semantic::TEXCOORD_0:
				{
					if ( stride == 0 )
					{
						stride = 16;//sizeof( Vec2 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = primitive.vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.t.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC2 );
					break;
				}
				case spot::gltf::Mesh::Primitive::Semantic::COLOR_0:
				{
					if ( stride == 0 )
					{
						stride = sizeof( Color );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = primitive.vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.c.r, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC4 );
					break;
				}
				default:
				{
					assert( false && "Semantic not supported" );
				}
				}
			}

			mesh.primitives.emplace_back( std::move( primitive ) );
		}

		meshes.emplace_back( std::move( mesh ) );
	}
}


} // namespace gfx
