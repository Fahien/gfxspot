#include "spot/gfx/models.h"

#include <cassert>
#include <spot/gltf/gltf.h>

#include "spot/gfx/graphics.h"


namespace mth = spot::math;
namespace gtf = spot::gltf;


namespace spot::gfx
{

const Color Color::white = { 1.0f, 1.0f, 1.0f };

Material Material::white = Material{
	Ubo{
		Color::white
	}
};


Mesh Mesh::create_line( const Vec3& a, const Vec3& b )
{
	Mesh ret;

	Primitive prim;

	prim.vertices.resize( 2 );
	prim.vertices[0].p = a;
	prim.vertices[1].p = b;

	prim.indices = { 0, 1 };

	ret.primitives.emplace_back( std::move( prim ) );

	return ret;
}


Mesh Mesh::create_triangle( const Vec3& a, const Vec3& b, const Vec3& c, Material* material )
{
	Mesh ret;

	Primitive prim;

	prim.vertices.resize( 3 );
	prim.vertices[0].p = a;
	prim.vertices[1].p = b;
	prim.vertices[2].p = c;

	prim.material = material;

	if ( material )
	{
		prim.indices = { 0, 1, 2 };
	}
	else
	{
		prim.indices = { 0, 1, 1, 2, 2, 0 };
	}

	ret.primitives.emplace_back( std::move( prim ) );

	return ret;
}


Mesh Mesh::create_rect( const Vec3& a, const Vec3& b, Material* material )
{
	Mesh ret;

	Primitive prim;

	prim.vertices.resize( 4 );
	prim.vertices[0].p = a;
	prim.vertices[1].p = Vec3( b.x, a.y );
	prim.vertices[2].p = b;
	prim.vertices[3].p = Vec3( a.x, b.y );

	prim.material = material;

	if ( material )
	{
		// .---B
		// A---`
		bool case1 = ( b.x > a.x && b.y > a.y );

		// ,---A
		// B---`
		bool case2 = ( b.x < a.x && b.y < a.y );

		if ( case1 || case2 )
		{
			prim.indices = { 0, 1, 2, 0, 2, 3 };
		}
		else
		{
			prim.indices = { 0, 2, 1, 0, 3, 2 };
		}
	}
	else
	{
		// No material, use lines
		prim.indices = { 0, 1, 1, 2, 2, 3, 3, 0 };
	}

	ret.primitives.emplace_back( std::move( prim ) );

	return ret;
}


Mesh Mesh::create_quad( const Vec3& a, const Vec3& b )
{
	Mesh ret = create_rect( a, b, &Material::white );

	auto& vertices = ret.primitives[0].vertices;

	// Text coords
	vertices[0].t = Vec2( 0.0f, 0.0 ); // a
	vertices[1].t = Vec2( 1.0f, 0.0 ); // b
	vertices[2].t = Vec2( 1.0f, 1.0 ); // c
	vertices[3].t = Vec2( 0.0f, 1.0 ); // d

	return ret;
}


Models::Models( Graphics& g )
: graphics { g }
, images{ g.device }
{}


int Models::create_node()
{
	auto& node = nodes.emplace_back();
	node.index = nodes.size() - 1;
	return node.index;
}


int Models::create_node( Mesh&& mesh )
{
	auto& node = nodes.emplace_back();
	node.index = nodes.size() - 1;

	meshes.emplace_back( std::move( mesh ) );
	node.mesh = meshes.size() - 1;

	return node.index;
}


gtf::Node* Models::get_node( const int index )
{
	assert( index >= 0 && index < nodes.size() && "Cannot get node out of bounds" );
	return &nodes[index];
}


gtf::Scene& Models::load( const std::string& path )
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
						stride = sizeof( Vec3 );
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
						stride = sizeof( Vec3 );
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
						stride = sizeof( Vec2 );
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

	nodes = std::move( model.nodes );
	scene = *model.scene;

	return scene;
}


} // namespace spot::gfx
