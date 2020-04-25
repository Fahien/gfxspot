#include "spot/gltf/mesh.h"

#include "spot/gltf/gltf.h"

namespace spot::gfx
{

Primitive::Primitive(
	std::vector<Vertex> v,
	std::vector<Index> i,
	const Handle<Material>& m
)
: vertices { std::move( v ) }
, indices { std::move( i ) }
, material { m }
{}


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


Mesh Mesh::create_triangle( const math::Vec3& a, const math::Vec3& b, const math::Vec3& c, const Handle<Material>& material )
{
	Mesh ret;

	std::vector<Vertex> vertices;
	vertices.resize( 3 );
	vertices[0].p = a;
	vertices[1].p = b;
	vertices[2].p = c;

	std::vector<Index> indices;
	if ( material )
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


Mesh Mesh::create_rect( const math::Rect& r, const Handle<Material>& material )
{
	auto a = math::Vec3( r.offset.x, r.offset.y );
	auto b = math::Vec3( r.extent.x + r.offset.x, r.extent.y + r.offset.y );
	return create_rect( a, b, material );
}


Mesh Mesh::create_rect( const math::Vec3& a, const math::Vec3& b, const Handle<Material>& material )
{
	Mesh ret;

	std::vector<Vertex> vertices;
	vertices.resize( 4 );
	vertices[0].p = a;
	vertices[1].p = math::Vec3( b.x, a.y, a.z );
	vertices[2].p = b;
	vertices[3].p = math::Vec3( a.x, b.y, a.z );

	std::vector<Index> indices;
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
	auto mesh = create_rect( a, b, Handle<Material>() );
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


Mesh Mesh::create_quad( const Handle<Material>& material, const math::Vec3& a, const math::Vec3& b )
{
	assert( material && "Cannot create a quad with invalid material" );
	Mesh ret = create_rect( a, b, material );

	auto& vertices = ret.primitives[0].vertices;

	// Text coords
	vertices[0].t = math::Vec2( 1.0f, 0.0 ); // a
	vertices[1].t = math::Vec2( 0.0f, 0.0 ); // b
	vertices[2].t = math::Vec2( 0.0f, 1.0 ); // c
	vertices[3].t = math::Vec2( 1.0f, 1.0 ); // d

	return ret;
}


Mesh::Mesh( Gltf& g )
: model { &g }
{}


Mesh::Mesh( Mesh&& other )
: model { other.model }
, primitives { std::move( other.primitives ) }
, weights { std::move( other.weights ) }
, name { std::move( other.name ) }
, extras { other.extras }
{
	other.model  = nullptr;
	other.extras = nullptr;
}


} // namespace spot::gfx
