#include <cstdlib>
#include <filesystem>
#include <cmath>
#include <spot/log.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"
#include "spot/gfx/animations.h"
#include "spot/gfx/gui.h"


int main( const int argc, const char** argv )
{
	using namespace spot;

	auto gfx = gfx::Graphics();

	auto model = gfx.models.push( gfx.device );

	gfx.window.on_resize = [&gfx]( const VkExtent2D& extent ) {
		gfx.viewport.set_extent( extent );
	};
	gfx.camera.set_perspective( gfx.viewport, math::radians( 60.0f ) );
	auto eye = math::Vec3::One * 4.0f;
	gfx.camera.look_at( eye, math::Vec3::Zero, math::Vec3::Y );

	auto node = model->nodes.push();

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		gfx.animations.update( dt, model );

		if ( gfx.window.scroll.y )
		{
			gfx.camera.node.translation += gfx.window.scroll.y;
		}

		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		gfx.gui.update( dt, gfx.window );

		if ( gfx.render_begin() )
		{
			gfx.draw( node );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}

