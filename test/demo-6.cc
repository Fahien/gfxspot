#include <cstdlib>
#include <spot/gfx/graphics.h>

/// Draw a rectangle
int main()
{
	using namespace spot::gfx;

	auto gfx = Graphics();

	auto a = Vec3(); // ( x=0.0f, y=0.0f, z=0.0f )
	auto b = Vec3( 4.5f, 2.0f ); // ( x=4.5f, y=2.0f, z=0.0f )

	auto& root = gfx.models.create_node(
		Mesh::create_rect( a, b )
	);

	auto eye = spot::math::Vec3( 0.0f, 0.0f, 8.0f ); // Out of the screen
	auto origin = spot::math::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = spot::math::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.render_begin() )
		{
			gfx.draw( root.index );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
