#include "spot/gfx/models.h"

#include <cassert>
#include <spot/gltf/gltf.h>

#include "spot/gfx/graphics.h"


namespace mth = spot::math;
namespace gtf = spot::gltf;


namespace spot::gfx
{

const Color Color::black = { 0.0f, 0.0f, 0.0f };
const Color Color::white = { 1.0f, 1.0f, 1.0f };
const Color Color::red = { 1.0f, 0.0f, 0.0f };
const Color Color::green = { 0.0f, 1.0f, 0.0f };
const Color Color::blue = { 0.0f, 0.0f, 1.0f };
const Color Color::yellow = { 1.0f, 1.0f, 0.0f };

Material& Material::get_black()
{
	static Material black {
		Ubo{
			Color::black
		}
	};
	return black;
};

Material& Material::get_white()
{
	static Material white {
		Ubo{
			Color::white
		}
	};
	return white;
};

Material& Material::get_red()
{
	static Material red = Material{
		Material::Ubo {
			Color::red
		}
	};
	return red;
}


Material& Material::get_yellow()
{
	static Material yellow = Material{
		Material::Ubo {
			Color::yellow
		}
	};
	return yellow;
}

Primitive::Primitive(
	const std::vector<Vertex>& vv,
	const std::vector<Index>& ii,
	int32_t mat )
: vertices { std::move( vv ) }
, indices { std::move( ii ) }
, material { mat }
{
}

Mesh Mesh::create_line( const Vec3& a, const Vec3& b, const Color& c, const float line_width )
{
	Mesh ret;

	Primitive prim;

	prim.vertices.resize( 2 );
	prim.vertices[0].p = a;
	prim.vertices[0].c = c;
	prim.vertices[1].p = b;
	prim.vertices[1].c = c;

	prim.indices = { 0, 1 };

	prim.line_width = line_width;

	ret.primitives.emplace_back( std::move( prim ) );

	return ret;
}


Mesh Mesh::create_triangle( const Vec3& a, const Vec3& b, const Vec3& c, const int32_t material )
{
	Mesh ret;

	std::vector<Vertex> vertices;
	vertices.resize( 3 );
	vertices[0].p = a;
	vertices[1].p = b;
	vertices[2].p = c;

	std::vector<Index> indices;
	if ( material >= 0 )
	{
		indices = { 0, 1, 2 };
	}
	else
	{
		indices = { 0, 1, 1, 2, 2, 0 };
	}

	ret.primitives.emplace_back(
		Primitive(
			std::move( vertices ),
			std::move( indices ),
			material )
	);

	return ret;
}


Mesh Mesh::create_rect( const Vec3& a, const Vec3& b, const int32_t material )
{
	Mesh ret;

	std::vector<Vertex> vertices;
	vertices.resize( 4 );
	vertices[0].p = a;
	vertices[1].p = Vec3( b.x, a.y, a.z );
	vertices[2].p = b;
	vertices[3].p = Vec3( a.x, b.y, a.z );

	std::vector<Index> indices;
	if ( material >= 0 )
	{
		// .---B
		// A---`
		bool case1 = ( b.x > a.x && b.y > a.y );

		// ,---A
		// B---`
		bool case2 = ( b.x < a.x && b.y < a.y );

		if ( case1 || case2 )
		{
			indices = { 0, 1, 2, 0, 2, 3 };
		}
		else
		{
			indices = { 0, 2, 1, 0, 3, 2 };
		}
	}
	else
	{
		// No material, use lines
		indices = { 0, 1, 1, 2, 2, 3, 3, 0 };
	}

	ret.primitives.emplace_back(
		Primitive(
			std::move( vertices ),
			std::move( indices ),
			material )
	);

	return ret;
}


Mesh Mesh::create_quad( const Vec3& a, const Vec3& b, const int32_t material )
{
	Mesh ret = create_rect( a, b, material );

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


gltf::Node& Models::create_node()
{
	auto& node = nodes.emplace_back();
	node.index = nodes.size() - 1;
	return node;
}


gltf::Node& Models::create_node( Mesh&& mesh )
{
	auto& node = nodes.emplace_back();
	node.index = nodes.size() - 1;

	meshes.emplace_back( std::move( mesh ) );
	node.mesh = meshes.size() - 1;

	return node;
}


gtf::Node* Models::get_node( const int32_t index )
{
	assert( index >= 0 && index < nodes.size() && "Cannot get node out of bounds" );
	return &nodes[index];
}


Material& Models::create_material( Material&& material )
{
	material.index = materials.size();
	return materials.emplace_back( std::move( material ) );
}


gfx::Material* Models::get_material( const int32_t index )
{
	if ( index >= 0 && index < materials.size() )
	{
		return &materials[index];
	}
	return nullptr;
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
			std::vector<Index> indices;

			// Indices
			if ( auto accessor = p.get_indices() )
			{
				indices.resize( accessor->count );
				std::memcpy( indices.data(), accessor->get_data(), accessor->get_size() );

				assert( accessor->component_type == spot::gltf::Accessor::ComponentType::UNSIGNED_SHORT );
				assert( accessor->type == spot::gltf::Accessor::Type::SCALAR );
			}

			// Vertex attributes
			std::vector<Vertex> vertices;

			auto attributes = p.get_attributes();
			for ( auto [semantic, accessor] : attributes )
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
				case gtf::Mesh::Primitive::Semantic::POSITION:
				{
					if ( stride == 0 )
					{
						stride = sizeof( Vec3 );
					}
				
					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.p.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC3 );
					break;
				}
				case gtf::Mesh::Primitive::Semantic::NORMAL:
				{
					if ( stride == 0 )
					{
						stride = sizeof( Vec3 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.n.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC3 );
					break;
				}
				case gtf::Mesh::Primitive::Semantic::TEXCOORD_0:
				{
					if ( stride == 0 )
					{
						stride = sizeof( Vec2 );
					}

					for ( size_t i = 0; i < accessor->count; ++i )
					{
						auto& vert = vertices[i];
						auto vert_data = data + i * stride;
						std::memcpy( &vert.t.x, vert_data, elem_size );
					}

					assert( accessor->component_type == spot::gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == spot::gltf::Accessor::Type::VEC2 );
					break;
				}
				case gtf::Mesh::Primitive::Semantic::COLOR_0:
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

			mesh.primitives.emplace_back(
				Primitive(
					std::move( vertices ),
					std::move( indices ),
					p.material )
			);
		}

		meshes.emplace_back( std::move( mesh ) );
	}

	nodes = std::move( model.nodes );
	scene = *model.scene;

	return scene;
}


} // namespace spot::gfx
