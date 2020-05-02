#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"
#include "spot/gfx/animations.h"


int main( const int argc, const char** argv )
{
	using namespace spot;
	auto gfx = gfx::Graphics();

	// Set viewport and camera
	gfx.window.on_resize = [&gfx]( const VkExtent2D& extent ) { gfx.viewport.set_extent( extent ); };
	gfx.camera.set_perspective( gfx.viewport, math::radians( 60.0f ) );
	auto eye = math::Vec3::One * 4.0f;
	gfx.camera.look_at( eye, math::Vec3::Zero, math::Vec3::Y );

	// Create scene
	auto model = gfx.models.push( gfx.device );

	auto blue_cube = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Blue ) )
		) )
	);

	auto red_cube = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Red ) )
		) )
	);
	red_cube->translation.x += 1.0f;

	// Loop
	while ( gfx.window.is_alive() )
	{
		// Input
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		// Update
		gfx.animations.update( dt, model );

		if ( gfx.window.scroll.y )
		{
			gfx.camera.node.translation += gfx.window.scroll.y;
		}

		if ( gfx.window.swipe.x != 0.0f )
		{
			blue_cube->rotation *= math::Quat( math::Vec3::Y, math::radians( gfx.window.swipe.x ) );
		}
		if ( gfx.window.swipe.y != 0.0f )
		{
			blue_cube->rotation *= math::Quat( math::Vec3::X, math::radians( -gfx.window.swipe.y ) );
		}
		// Render
		if ( gfx.render_begin() )
		{
			gfx.draw( blue_cube );
			gfx.draw( red_cube );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
