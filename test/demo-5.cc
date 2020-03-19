#include <cstdlib>
#include <filesystem>
#include <spot/math/math.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{

int create_line( gfx::Graphics& graphics, gfx::Dot a, gfx::Dot b )
{
	auto& node = graphics.models.create_node();

	auto& mesh = graphics.models.meshes.emplace_back();
	node.mesh = graphics.models.meshes.size() - 1;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c )
	};

	primitive.indices = { 0, 1 };

	return node.index;
}


int create_triangle( gfx::Graphics& graphics, gfx::Dot a, gfx::Dot b, gfx::Dot c )
{
	auto& node = graphics.models.create_node();

	auto& mesh = graphics.models.meshes.emplace_back();
	node.mesh = graphics.models.meshes.size() - 1;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c ),
		gfx::Vertex( c.p, c.c )
	};

	primitive.indices = { 0, 1, 1, 2, 2, 0 };

	return node.index;
}


void rotate( Node& n, float angle )
{
	const math::Vec3 axis = { 0.0f, 0.0f, 1.0f };
	auto rot_axis = math::Quat( axis, angle );
	n.rotation *= rot_axis;
}

} // namespace spot::gfx

int main( const int argc, const char** argv )
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	Scene* scene;
	if ( argc > 1 )
	{
		auto path = std::string( argv[1] );
		scene = &graphics.models.load( path );
	}

	auto x = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 1.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	
	auto y = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 0.0f, 1.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

	auto z = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 1.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );

	auto triangle = create_triangle( graphics,
		Dot( Vec3( 0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( -0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 0.0f ) ) );
	
	spot::math::Vec3 eye = { 1.5f, 1.5f, 1.5f };
	spot::math::Vec3 zero = {};
	spot::math::Vec3 up = { 0.0f, 1.0f, 0.0f };
	graphics.view = look_at( eye, zero, up );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();
		graphics.window.update( dt );

		if ( graphics.window.swipe.x != 0 )
		{
			auto angle = spot::math::radians( graphics.window.swipe.x );
			rotate( *graphics.models.get_node( x ), angle );
			rotate( *graphics.models.get_node( y ), angle );
			rotate( *graphics.models.get_node( z ), angle );
			rotate( *graphics.models.get_node( triangle ), angle );
		}

		if ( graphics.window.scroll.y != 0 )
		{
			graphics.models.get_node( 2 )->translation.y += graphics.window.scroll.y;
		}

		if ( graphics.render_begin() )
		{
			graphics.draw( x );
			graphics.draw( y );
			graphics.draw( z );
			if ( scene )
			{
				graphics.draw( *scene );
			}
			else
			{
				graphics.draw( triangle );
			}

			graphics.render_end();
		}
	}

	return EXIT_SUCCESS;
}
