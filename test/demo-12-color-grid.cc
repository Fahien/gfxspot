#include <cstdlib>

#include <spot/gfx/mesh.h>
#include <spot/gfx/graphics.h>

constexpr size_t width = 64;
constexpr size_t height = width;

namespace spot::gfx
{

Handle<Node> create_grid( Model& model )
{
	using namespace spot::math;

	auto grid = model.nodes.push();

	float step = width / 6.0f;

	for ( size_t j = 0; j < height; ++j )
	{
		for ( size_t i = 0; i < width; ++i )
		{
			auto color = Color();

			if ( i < step )
			{
				color.r = 1.0f;
				color.g = i / step; // increase gradually
			}
			else if ( i >= step && i < 2 * step )
			{
				color.r = 1.0f - ( i - step ) / step;
				color.g = 1.0f;
			}
			else if ( i >= 2 * step && i < 3 * step )
			{
				color.g = 1.0f;
				color.b = ( i - 2 * step ) / step; // increase gradually
			}
			else if ( i >= 3 * step && i < 4 * step )
			{
				color.g = 1.0f - ( i - 3 * step ) / step; // decrease gradually
				color.b = 1.0f;
			}
			else if ( i >= 4 * step && i < 5 * step )
			{
				color.b = 1.0f;
				color.r = ( i - 4 * step ) / step;
			}
			else if ( i >= 5 * step )
			{
				color.b = 1.0f - ( i - 5 * step ) / step; // decrease gradually
				color.r = 1.0f;
			}

			color.r += j / float( height );
			color.b += j / float( height );
			color.g += j / float( height );

			auto material = model.materials.push( Material( color ) );
			auto mesh = model.meshes.push(
				Mesh::create_rect(
					Rect( Vec2::Zero, Vec2::One ),
					material
				)
			);
			auto cell = model.nodes.push( Node( mesh ) );
			cell->translation.x += i;
			cell->translation.y += j;
			grid->add_child( cell );
		}
	}

	return grid;
}

} // namespace gfx


// Draw a color spectrum grid
int main()
{
	using namespace spot;
	using namespace spot::math;

	auto gfx = gfx::Graphics();

	gfx.viewport.set_extent( width, height );
	gfx.viewport.set_offset( Vec2::Zero );

	auto grid = create_grid( *gfx.models.push( gfx::Model( gfx.device ) ) );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();

		if ( gfx.render_begin() )
		{
			gfx.draw( *grid );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
