#include "spot/gltf/mesh.h"

#include "spot/gfx/model.h"
#include "spot/gltf/node.h"


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
	auto a = math::Vec3( r.a );
	auto b = math::Vec3( r.b );
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
	auto a = math::Vec3( r.a );
	auto b = math::Vec3( r.b );
	return create_rect( a, b, color );
}


Mesh Mesh::create_quad( const Handle<Material>& material, const math::Vec3& a, const math::Vec3& b )
{
	assert( material && "Cannot create a quad with invalid material" );
	Mesh ret = create_rect( a, b, material );

	auto& vertices = ret.primitives[0].vertices;

	// Text coords
	vertices[0].t = math::Vec2( 0.0f, 1.0 ); // a
	vertices[1].t = math::Vec2( 1.0f, 1.0 ); // b
	vertices[2].t = math::Vec2( 1.0f, 0.0 ); // c
	vertices[3].t = math::Vec2( 0.0f, 0.0 ); // d

	return ret;
}


Mesh Mesh::create_cube( const Handle<Material>& material )
{
	auto v = std::vector<Vertex>( 24 );

	// Front
	v[0].p = math::Vec3( -0.5f, -0.5, 0.5f );
	v[0].n = math::Vec3::Z;
	v[0].t = math::Vec2( 1 / 3.0f, 0.25f );
	v[1].p = math::Vec3( 0.5f, -0.5, 0.5f );
	v[1].n = math::Vec3::Z;
	v[1].t = math::Vec2( 2 / 3.0f, 0.25f );
	v[2].p = math::Vec3( 0.5f, 0.5, 0.5f );
	v[2].n = math::Vec3::Z;
	v[2].t = math::Vec2( 2 / 3.0f, 0.5f );
	v[3].p = math::Vec3( -0.5f, 0.5, 0.5f );
	v[3].n = math::Vec3::Z;
	v[3].t = math::Vec2( 1 / 3.0f, 0.5f );

	// Right
	v[4].p = math::Vec3( 0.5f, -0.5, 0.5f );
	v[4].n = math::Vec3::X;
	v[4].t = math::Vec2( 2 / 3.0f, 0.25f );
	v[5].p = math::Vec3( 0.5f, -0.5, -0.5f );
	v[5].n = math::Vec3::X;
	v[5].t = math::Vec2( 1.0f, 0.25f );
	v[6].p = math::Vec3( 0.5f, 0.5, -0.5f );
	v[6].n = math::Vec3::X;
	v[6].t = math::Vec2( 1.0f, 0.5f );
	v[7].p = math::Vec3( 0.5f, 0.5, 0.5f );
	v[7].n = math::Vec3::X;
	v[7].t = math::Vec2( 2 / 3.0f, 0.5f );

	// Back
	v[8].p  = math::Vec3( 0.5f, -0.5, -0.5f );
	v[8].n  = -math::Vec3::Z;
	v[8].t  = math::Vec2( 1 / 3.0f, 0.75f );
	v[9].p  = math::Vec3( -0.5f, -0.5, -0.5f );
	v[9].n  = -math::Vec3::Z;
	v[9].t  = math::Vec2( 2 / 3.0f, 0.75f );
	v[10].p = math::Vec3( -0.5f, 0.5, -0.5f );
	v[10].n = -math::Vec3::Z;
	v[10].t = math::Vec2( 2 / 3.0f, 1.0f );
	v[11].p = math::Vec3( 0.5f, 0.5, -0.5f );
	v[11].n = -math::Vec3::Z;
	v[11].t = math::Vec2( 1 / 3.0f, 1.0f );

	// Left
	v[12].p = math::Vec3( -0.5f, -0.5, -0.5f );
	v[12].n = -math::Vec3::X;
	v[12].t = math::Vec2( 0.0f, 0.25f );
	v[13].p = math::Vec3( -0.5f, -0.5, 0.5f );
	v[13].n = -math::Vec3::X;
	v[13].t = math::Vec2( 1 / 3.0f, 0.25f );
	v[14].p = math::Vec3( -0.5f, 0.5, 0.5f );
	v[14].n = -math::Vec3::X;
	v[14].t = math::Vec2( 1 / 3.0f, 0.5f );
	v[15].p = math::Vec3( -0.5f, 0.5, -0.5f );
	v[15].n = -math::Vec3::X;
	v[15].t = math::Vec2( 0.0f, 0.5f );

	// Top
	v[16].p = math::Vec3( -0.5f, 0.5, 0.5f );
	v[16].n = math::Vec3::Y;
	v[16].t = math::Vec2( 1 / 3.0f, 0.5f );
	v[17].p = math::Vec3( 0.5f, 0.5, 0.5f );
	v[17].n = math::Vec3::Y;
	v[17].t = math::Vec2( 2 / 3.0f, 0.5f );
	v[18].p = math::Vec3( 0.5f, 0.5, -0.5f );
	v[18].n = math::Vec3::Y;
	v[18].t = math::Vec2( 2 / 3.0f, 0.75f );
	v[19].p = math::Vec3( -0.5f, 0.5, -0.5f );
	v[19].n = math::Vec3::Y;
	v[19].t = math::Vec2( 1 / 3.0f, 0.75f );

	// Bottom
	v[20].p = math::Vec3( -0.5f, -0.5, -0.5f );
	v[20].n = -math::Vec3::Y;
	v[20].t = math::Vec2( 1 / 3.0f, 0.0f );
	v[21].p = math::Vec3( 0.5f, -0.5, -0.5f );
	v[21].n = -math::Vec3::Y;
	v[21].t = math::Vec2( 2 / 3.0f, 0.0f );
	v[22].p = math::Vec3( 0.5f, -0.5, 0.5f );
	v[22].n = -math::Vec3::Y;
	v[22].t = math::Vec2( 2 / 3.0f, 0.25f );
	v[23].p = math::Vec3( -0.5f, -0.5, 0.5f );
	v[23].n = -math::Vec3::Y;
	v[23].t = math::Vec2( 1 / 3.0f, 0.25f );

	auto i = std::vector<Index> {
		0, 1, 2, 0, 2, 3, // front face
		4, 5, 6, 4, 6, 7, // right
		8, 9, 10, 8, 10, 11, // back
		12, 13, 14, 12, 14, 15, // left
		16, 17, 18, 16, 18, 19, // top
		20, 21, 22, 20, 22, 23, // bottom
	};
	
	auto p = Primitive( std::move( v ), std::move( i ), material );

	return Mesh( { std::move( p ) } );
}


} // namespace spot::gfx
