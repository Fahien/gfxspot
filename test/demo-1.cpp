#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "graphics/graphics.hpp"
#include "graphics/png.h"
#include "graphics/images.h"


void update( const double dt, gfx::Triangle& r )
{
	r.ubo.model.rotateY( mth::radians( dt * 16.0 ) );
}


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.rotateZ( -mth::radians( dt * 16.0 ) );
}


gfx::Mesh create_quad()
{
	using namespace gfx;

	Mesh quad;

	quad.vertices = {
		Vertex(
			Point( -0.5f, -0.5f, 0.0f ),
			Color( 0.3f, 0.0f, 0.0f, 0.5f ),
			Coord( 0.0f, 0.0 ) // a
		),
		Vertex(
			Point( 0.5f, -0.5f, 0.0f ),
			Color( 0.0f, 0.3f, 0.0f, 0.5f ),
			Coord( 1.0f, 0.0 ) // b
		),
		Vertex(
			Point( -0.5f, 0.5f, 0.0f ),
			Color( 0.3f, 0.0f, 0.3f, 0.5f ),
			Coord( 0.0f, 1.0 ) // d
		),
		Vertex(
			Point( 0.5f, 0.5f, 0.0f ),
			Color( 0.0f, 0.0f, 0.3f, 0.5f ),
			Coord( 1.0f, 1.0 ) // c
		),
	};

	// Currently, counterclockwise?
	quad.indices = { 0, 2, 1, 1, 2, 3 };

	return quad;
}


int main()
{
	using namespace gfx;

	auto graphics = Graphics();

	auto view = graphics.images.load( "img/lena.png" );
	auto quad = create_quad();
	quad.image_view = view;

	auto square = Rect(
		Dot( Point( -0.5f, -0.5f ) ),
		Dot( Point( 0.5f, 0.5f ) ) );
	
	auto triangle = Triangle(
		Dot( Point( 0.5f, 0.0f, -1.0f ) ),
		Dot( Point( -0.5f, 0.0f, -1.0f ) ),
		Dot( Point( 0.0f, 0.0f, 0.0f ) ) );

	graphics.renderer.add( square );
	graphics.renderer.add( triangle );
	graphics.renderer.add( quad );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update(dt, triangle.ubo);
		update(dt, square.ubo);
		update(dt, quad.ubo);

		if ( graphics.render_begin() )
		{
			graphics.draw( quad );
			graphics.draw( square );
			graphics.draw( triangle );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
