#include <cstdlib>
#include <spot/gfx/graphics.h>

/// Draw a rectangle
int main()
{
	using namespace spot;

	auto gfx = gfx::Graphics();
	auto model = gfx.create_model();

	auto a = math::Vec3::One;
	auto b = math::Vec3( 4.5f, 2.0f ); // ( x=4.5f, y=2.0f, z=0.0f )

	auto root = model->nodes.push( gfx::Node(
		model->meshes.push( gfx::Mesh::create_rect( a, b ) )
	) );

	gfx.camera.look_at( math::Vec3::Z * 2.0f, math::Vec3::Zero, math::Vec3::Y );
	gfx.viewport.set_offset( 0.0f, 0.0f );
	gfx.viewport.set_extent( 8.0f, 8.0f );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.render_begin() )
		{
			gfx.draw( root );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();

	return EXIT_SUCCESS;
}
