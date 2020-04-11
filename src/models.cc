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


Mesh Mesh::create_line( const math::Vec3& a, const math::Vec3& b, const Color& c, const float line_width )
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


Mesh Mesh::create_triangle( const math::Vec3& a, const math::Vec3& b, const math::Vec3& c, const int32_t material )
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


Mesh Mesh::create_rect( const math::Rect& r, int32_t material )
{
	auto a = math::Vec3( r.offset.x, r.offset.y );
	auto b = math::Vec3( r.extent.x + r.offset.x, r.extent.y + r.offset.y );
	return create_rect( a, b, material );
}


Mesh Mesh::create_rect( const math::Vec3& a, const math::Vec3& b, const int32_t material )
{
	Mesh ret;

	std::vector<Vertex> vertices;
	vertices.resize( 4 );
	vertices[0].p = a;
	vertices[1].p = math::Vec3( b.x, a.y, a.z );
	vertices[2].p = b;
	vertices[3].p = math::Vec3( a.x, b.y, a.z );

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


Mesh Mesh::create_rect( const math::Vec3& a, const math::Vec3& b, const Color& color )
{
	auto mesh = create_rect( a, b, -1 );
	for ( auto& prim : mesh.primitives )
	{
		for ( auto& vert : prim.vertices )
		{
			vert.c = color;
		}
	}
	return mesh;
}


Mesh Mesh::create_rect( const math::Rect& r, const Color& color )
{
	auto a = math::Vec3( r.offset.x, r.offset.y );
	auto b = math::Vec3( r.extent.x + r.offset.x, r.extent.y + r.offset.y );
	return create_rect( a, b, color );
}


Mesh Mesh::create_quad( const int32_t material, const math::Vec3& a, const math::Vec3& b )
{
	Mesh ret = create_rect( a, b, material );

	auto& vertices = ret.primitives[0].vertices;

	// Text coords
	vertices[0].t = math::Vec2( 1.0f, 0.0 ); // a
	vertices[1].t = math::Vec2( 0.0f, 0.0 ); // b
	vertices[2].t = math::Vec2( 0.0f, 1.0 ); // c
	vertices[3].t = math::Vec2( 1.0f, 1.0 ); // d

	return ret;
}


Models::Models( Graphics& g )
: graphics { g }
, images{ g.device }
{}


gltf::Node& Models::create_node( const int32_t parent )
{
	return gltf.create_node( parent );
}


gltf::Node& Models::create_node( Mesh&& mesh, const int32_t parent )
{
	auto& node = gltf.create_node( parent );

	meshes.emplace_back( std::move( mesh ) );
	node.mesh = meshes.size() - 1;

	return node;
}


gltf::Node* Models::get_node( const int32_t node )
{
	return gltf.get_node( node );
}


Material& Models::create_material( const Color& c )
{
	auto& material = create_material();
	material.ubo.color = c;
	return material;
}


Material& Models::create_material( Material&& material )
{
	material.index = materials.size();
	return materials.emplace_back( std::move( material ) );
}


Material& Models::create_material( VkImageView texture )
{
	auto& material = create_material();
	material.texture = texture;
	return material;
}


Material* Models::get_material( const int32_t index )
{
	if ( index >= 0 && index < materials.size() )
	{
		return &materials[index];
	}
	return nullptr;
}


Mesh& Models::create_mesh( Mesh&& mesh )
{
	mesh.index = meshes.size();
	auto& ret = meshes.emplace_back( std::move( mesh ) );
	return ret;
}


/// @todo Implement
int32_t Models::create_text( const std::string& text )
{
	int32_t group = gltf.create_node().index;
	// For each character in the text
	for ( auto c : text )
	{
		// We need to create a node
		auto& node = gltf.create_node( group );
		// Its mesh will be a quad
		// Quad's material will be the same for each character
		// Material will have a texture to the bitmap font
		// Only texture coordinates will change between each different character
	}
	return group;
}


gltf::Scene& Models::load( const std::string& path )
{
	gltf = gltf::Gltf::load( path );

	// Load materials
	for ( auto& m : gltf.materials )
	{
		Material material;

		material.ubo.color.r = m.pbr.base_color_factor[0];
		material.ubo.color.g = m.pbr.base_color_factor[1];
		material.ubo.color.b = m.pbr.base_color_factor[2];
		material.ubo.color.a = m.pbr.base_color_factor[3];

		material.ubo.metallic = m.pbr.metallic_factor;
		material.ubo.roughness = m.pbr.roughness_factor;

		if ( auto texture = m.get_texture() )
		{
			auto source = texture->get_source();
			assert( source && "Texture has no source" );
			material.texture = images.load( source->uri.c_str() );
		}

		create_material( std::move( material ) );
	}

	// A primitive without material does not exist in gltf
	// Therefore we a white material at the endMaterial white {
	
	auto& white = create_material( Material{ Material::Ubo{ Color::white } } );

	// Load meshes
	for ( auto& m : gltf.meshes )
	{
		Mesh mesh;

		for ( auto& p : m.primitives )
		{
			// Check valid material
			if ( p.material < 0 )
			{
				p.material = white.index;
			}

			std::vector<Index> indices;

			// Indices
			if ( auto accessor = p.get_indices() )
			{
				indices.resize( accessor->count );
				std::memcpy( indices.data(), accessor->get_data(), accessor->get_size() );

				assert( accessor->component_type == gltf::Accessor::ComponentType::UNSIGNED_SHORT );
				assert( accessor->type == gltf::Accessor::Type::SCALAR );
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
				case gltf::Mesh::Primitive::Semantic::POSITION:
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

					assert( accessor->component_type == gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == gltf::Accessor::Type::VEC3 );
					break;
				}
				case gltf::Mesh::Primitive::Semantic::NORMAL:
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

					assert( accessor->component_type == gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == gltf::Accessor::Type::VEC3 );
					break;
				}
				case gltf::Mesh::Primitive::Semantic::TEXCOORD_0:
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

					assert( accessor->component_type == gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == gltf::Accessor::Type::VEC2 );
					break;
				}
				case gltf::Mesh::Primitive::Semantic::COLOR_0:
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

					assert( accessor->component_type == gltf::Accessor::ComponentType::FLOAT );
					assert( accessor->type == gltf::Accessor::Type::VEC4 );
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

	return *gltf.scene;
}


} // namespace spot::gfx
