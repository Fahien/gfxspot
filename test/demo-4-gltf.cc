#include <cstdlib>
#include <filesystem>
#include <cmath>
#include <spot/log.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"
#include "spot/gfx/animations.h"


int main( const int argc, const char** argv )
{
	using namespace spot;

	if ( argc < 2 )
	{
		loge( "Usage: %s <gltf>\n", argv[0] );
		return EXIT_FAILURE;
	}
	
	auto gfx = gfx::Graphics();

	auto path = std::string( argv[1] );
	auto model = gfx.load_model( path );

	gfx.window.on_resize = [&gfx]( const VkExtent2D& extent ) {
		gfx.viewport.set_extent( extent );
	};
	gfx.camera.set_perspective( gfx.viewport, math::radians( 60.0f ) );
	auto eye = math::Vec3::One * 4.0f;
	gfx.camera.look_at( eye, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();
		gfx.animations.update( dt, model );

		if ( gfx.window.scroll.y )
		{
			gfx.camera.node.translation += gfx.window.scroll.y;
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( model );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
