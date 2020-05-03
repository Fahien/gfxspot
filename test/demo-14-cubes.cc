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

	auto cube_root = model->nodes.push();

	auto dice = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push(
				model->images.load( "img/dice.png" )
			)
		) )
	);
	dice->translation.x -= 1.5f;
	cube_root->add_child( dice );

	auto gray_table = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Gray ) )
		))
	);
	gray_table->scale.x *= 6.0f;
	gray_table->scale.z *= 6.0f;
	gray_table->translation.y -= 1.0f;
	cube_root->add_child( gray_table );

	auto red_cube = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Red ) )
		) )
	);
	red_cube->translation.x += 1.5f;
	cube_root->add_child( red_cube );

	auto blue_cube = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Blue ) )
		) )
	);
	blue_cube->translation.z += 1.5f;
	cube_root->add_child( blue_cube );

	auto green_cube = model->nodes.push(
		model->meshes.push( gfx::Mesh::create_cube(
			model->materials.push( gfx::Material( gfx::Color::Green ) )
		) )
	);
	green_cube->translation.z -= 1.5f;
	cube_root->add_child( green_cube );

	gfx.light_node = model->nodes.push();
	gfx.light_node->light = model->lights.push();
	gfx.light_node->translation.y += 4.0f;
	gfx.renderer.add( gfx.light_node );

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

		cube_root->rotation *= math::Quat( math::Vec3::Y, math::radians( 4.0f * dt ) );

		if ( gfx.window.press.left )
		{
			cube_root->rotation *= math::Quat( math::Vec3::X, math::radians( -gfx.window.swipe.y * 4.0f * dt ) );
			cube_root->rotation *= math::Quat( math::Vec3::Y, math::radians( gfx.window.swipe.x * dt ) );
		}
		if ( gfx.window.press.right )
		{
			gfx.light_node->translation.y += gfx.window.swipe.y * dt;
		}

		gfx.ambient.ubo.strength += gfx.window.scroll.y * dt;

		// Render
		if ( gfx.render_begin() )
		{
			gfx.draw( cube_root );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
