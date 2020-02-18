#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;
namespace gtf = spot::gltf;


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.rotateZ( -mth::radians( dt * 16.0 ) );
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto x = Line(
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 1.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	graphics.renderer.add( x );
	
	auto y = Line(
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 0.0f, 1.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );
	graphics.renderer.add( y );

	auto z = Line(
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 1.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
	graphics.renderer.add( z );

	auto triangle = Triangle(
		Dot( Vec3( 0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( -0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 0.0f ) ) );
	graphics.renderer.add( triangle );

	graphics.view = look_at(
		{ 0.0f, 2.0f, 2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update( dt, triangle.ubo );

		if ( graphics.window.swipe.x != 0 )
		{
			
		}

		if ( graphics.render_begin() )
		{
			graphics.draw( x );
			graphics.draw( y );
			graphics.draw( z );
			graphics.draw( triangle );

			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
