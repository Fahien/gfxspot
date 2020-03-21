#include <cstdlib>
#include <spot/gfx/graphics.h>

namespace spot::gfx
{

struct Colors
{
	int32_t black;
	int32_t white;
	int32_t red;
	int32_t green;
	int32_t blue;
	int32_t yellow;
};


Colors create_colors( spot::gfx::Graphics& gfx )
{
	Colors ret;

	auto& black = gfx.models.create_material();
	black.ubo.color = Color( 30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f );
	ret.black = black.index;

	auto& white = gfx.models.create_material();
	white.ubo.color = Color( 1.0f, 1.0f, 1.0f );
	ret.white = white.index;

	auto& red = gfx.models.create_material();
	red.ubo.color = Color( 190.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f );
	ret.red = red.index;

	auto& green = gfx.models.create_material();
	green.ubo.color = Color( 0.0f, 1.0f, 0.0f );
	ret.green = green.index;

	auto& blue = gfx.models.create_material();
	blue.ubo.color = Color( 0.0f, 0.0f, 1.0f );
	ret.blue = blue.index;

	auto& yellow = gfx.models.create_material();
	yellow.ubo.color = Color( 230.0f / 255.0f, 200.0f / 255.0f, 100.0f / 255.0f );
	ret.yellow = yellow.index;

	return ret;
}

const float screen_width = 2.0f;
const float width = screen_width / 128.0f;
const float step = width * 2.0f;


uint32_t create_lines( const Colors& colors, Graphics& gfx )
{
	auto root_index = gfx.models.create_node().index;

	// First half
	// White lines
	for ( float x = -1.0f; x < 0.0f; x += step )
	{
		// Top left
		auto a = math::Vec3( x, -1.0f, 0.0f );
		// Bottom right
		auto b = math::Vec3( x + width, 1.0f, 0.0f );

		auto line = Mesh::create_rect( a, b, colors.white );

		auto& node = gfx.models.create_node( std::move( line ) );
		auto root = gfx.models.get_node( root_index );
		root->children.push_back( node.index );
	}
	// + black background
	{
		auto a = math::Vec3( -1.0f, -1.0f, -0.1f );
		auto b = math::Vec3( 0.0f, 1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.black );
		
		auto& node = gfx.models.create_node( std::move( line ) );
		auto root = gfx.models.get_node( root_index );
		root->children.push_back( node.index );
	}

	// Second half - top
	// Yellow lines
	for ( float y = -1.0f; y < 0.0f; y += step )
	{
		auto a = math::Vec3( 0.0, y, 0.0f );
		auto b = math::Vec3( 1.0f, y + width, 0.0f );

		auto line = Mesh::create_rect( a, b, colors.yellow );
		
		auto& node = gfx.models.create_node( std::move( line ) );
		auto root = gfx.models.get_node( root_index );
		root->children.push_back( node.index );
	}
	// + white background
	{
		auto a = math::Vec3( 0.0f, 0.0f - width, -0.1f );
		auto b = math::Vec3( 1.0f, -1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.white );
		auto& node = gfx.models.create_node( std::move( line ) );
		auto root = gfx.models.get_node( root_index );
		root->children.push_back( node.index );
	}
	// Black delimitator
	{
		auto a = math::Vec3( 0.0f, 0.0f, -0.1f );
		auto b = math::Vec3( 1.0f, -width, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.black );
		auto& node = gfx.models.create_node( std::move( line ) );
		auto root = gfx.models.get_node( root_index );
		root->children.push_back( node.index );
	}

	return root_index;
}


int32_t create_red_lines( const Colors& colors, Graphics& gfx )
{
	auto& root = gfx.models.create_node();

	float red_width = width * sqrtf( 2.0f );
	float step = red_width * 2;

	// Second half - bottom
	// Red lines
	for ( float x = 0.0f; x < sqrtf( 2.0f ); x += step )
	{
		// Bottom left
		auto a = math::Vec3( x, -1.0f, -0.2f );
		// Top right
		auto b = math::Vec3( x + red_width, 1.0f, -0.2f );

		auto line = Mesh::create_rect( a, b, colors.red );

		auto& node = gfx.models.create_node( std::move( line ) );
		root.children.emplace_back( node.index );
	}
	// + black background
	{
		auto a = math::Vec3( 0.0f, -1.0f, -0.3f );
		auto b = math::Vec3( 2.0f, 1.0f, -0.3f );
		auto line = Mesh::create_rect( a, b, colors.white );
		auto& node = gfx.models.create_node( std::move( line ) );
		root.children.emplace_back( node.index );
	}

	return root.index;
}


} // namespace spot::gfx


/// Draw lines
int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto gfx = Graphics();

	Colors colors = create_colors( gfx );

	auto lines = create_lines( colors , gfx );
	auto red_lines = create_red_lines( colors , gfx );

	auto eye = math::Vec3( 0.0f, 0.0f, -1.0f ); // Out of the screen
	auto origin = math::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = math::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	gfx.proj = ortho( -1.0f, 1.0, -1.0, 1.0, 0.125f, 2.0 );

	const auto rot45 = math::Mat4::identity.rotateZ( math::radians( 45.0f ) );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.render_begin() )
		{
			gfx.draw( lines );
			gfx.draw( red_lines, rot45 );

			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
