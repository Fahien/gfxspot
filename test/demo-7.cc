#include <cstdlib>
#include <spot/gfx/graphics.h>

namespace spot::gfx
{

const float screen_width = 2.0f;
const float width = screen_width / 128.0f;
const float step = width * 2.0f;

std::vector<Mesh> create_lines()
{
	std::vector<Mesh> ret;

	// First half
	// White lines
	for ( float x = -1.0f; x < 0.0f; x += step )
	{
		// Top left
		auto a = Vec3( x, -1.0f, 0.0f );
		// Bottom right
		auto b = Vec3( x + width, 1.0f, 0.0f );

		auto line = Mesh::create_rect( a, b, &Material::get_white() );

		ret.emplace_back( std::move( line ) );
	}
	// + black background
	{
		auto a = Vec3( -1.0f, -1.0f, -0.1f );
		auto b = Vec3( 0.0f, 1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, &Material::get_black() );
		ret.emplace_back( std::move( line ) );
	}

	// Second half - top
	// Yellow lines
	for ( float y = -1.0f; y < 0.0f; y += step )
	{
		auto a = Vec3( 0.0, y, 0.0f );
		auto b = Vec3( 1.0f, y + width, 0.0f );

		auto line = Mesh::create_rect( a, b, &Material::get_yellow() );
		
		ret.emplace_back( std::move( line ) );
	}
	// + black background
	{
		auto a = Vec3( 0.0f, 0.0f, -0.1f );
		auto b = Vec3( 1.0f, -1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, &Material::get_black() );
		ret.emplace_back( std::move( line ) );
	}

	return ret;
}


std::vector<Mesh> create_red_lines()
{
	std::vector<Mesh> ret;

	float red_width = width * sqrtf( 2.0f );
	float step = red_width * 2;

	// Second half - bottom
	// Red lines
	for ( float x = 0.0f; x < sqrtf( 2.0f ); x += step )
	{
		// Bottom left
		auto a = Vec3( x, -1.0f, -0.2f );
		// Top right
		auto b = Vec3( x + red_width, 1.0f, -0.2f );

		auto line = Mesh::create_rect( a, b, &Material::get_red() );

		ret.emplace_back( std::move( line ) );
	}
	// + black background
	{
		auto a = Vec3( 0.0f, -1.0f, -0.3f );
		auto b = Vec3( 2.0f, 1.0f, -0.3f );
		auto line = Mesh::create_rect( a, b, &Material::get_black() );
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
	std::vector<Mesh> red_lines = create_red_lines();

	auto eye = mth::Vec3( 0.0f, 0.0f, 1.0f ); // Out of the screen
	auto origin = mth::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = mth::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	gfx.proj = ortho( -1.0f, 1.0, -1.0, 1.0, 0.125f, 2.0 );

	const auto rot45 = mth::Mat4::identity.rotateZ( mth::radians( 45.0f ) );

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

			for ( auto& line : red_lines )
			{
				gfx.draw( line, rot45 );
			}

			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
