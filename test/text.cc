#include <cstdlib>

#include <spot/gfx/graphics.h>

int main()
{
	using namespace spot;

	auto gfx = gfx::Graphics();

	uint32_t text = gfx.models.create_text( "Hello" );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.render_begin() )
		{
			gfx.draw( text );

			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
