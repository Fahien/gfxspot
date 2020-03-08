#include <cstdlib>
#include <spot/gfx/graphics.h>

namespace spot::gfx
{

std::vector<Mesh> create_lines()
{
	std::vector<Mesh> ret;

	for ( size_t x = 0; x < 2; ++x )
	{
		auto a = Vec3( -0.5f + x * 0.5f, 1.0f );
		auto b = Vec3( -0.5f + x * 0.5f, -1.0f );

		auto line = Mesh::create_line( a, b );

		ret.emplace_back( std::move( line ) );
	}

	return ret;
}

} // namespace spot::gfx


/// Draw lines
int main()
{
	using namespace spot::gfx;

	auto gfx = Graphics();

	std::vector<Mesh> lines = create_lines();

	auto eye = mth::Vec3( 0.0f, 0.0f, 1.0f ); // Out of the screen
	auto origin = mth::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = mth::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	gfx.proj = ortho( -1.0f, 1.0, -1.0, 1.0, 0.125f, 2.0 );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.render_begin() )
		{
			for ( auto& line : lines )
			{
				gfx.draw( line );
			}
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
