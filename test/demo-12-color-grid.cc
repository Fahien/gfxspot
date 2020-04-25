#include <cstdlib>

#include <spot/gfx/graphics.h>

// Draw a color spectrum grid
int main()
{
	auto gfx = spot::gfx::Graphics();

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();

		if ( gfx.render_begin() )
		{
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
