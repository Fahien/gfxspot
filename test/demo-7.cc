#include <cstdlib>
#include <spot/gfx/graphics.h>

#define NODE( index ) ( gfx.models.get_node( index ) )

namespace spot::gfx
{

struct Colors
{
	Handle<Material> black;
	Handle<Material> white;
	Handle<Material> red;
	Handle<Material> green;
	Handle<Material> blue;
	Handle<Material> yellow;
};


Colors create_colors( Handle<Gltf>& model )
{
	Colors ret;

	auto black = model->materials.push();
	black->pbr.color = Color( 30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f );
	ret.black = black;

	auto white = model->materials.push();
	white->pbr.color = Color( 1.0f, 1.0f, 1.0f );
	ret.white = white;

	auto red = model->materials.push();
	red->pbr.color = Color( 190.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f );
	ret.red = red;

	auto green = model->materials.push();
	green->pbr.color = Color( 0.0f, 1.0f, 0.0f );
	ret.green = green;

	auto blue = model->materials.push();
	blue->pbr.color = Color( 0.0f, 0.0f, 1.0f );
	ret.blue = blue;

	auto yellow = model->materials.push();
	yellow->pbr.color = Color( 230.0f / 255.0f, 200.0f / 255.0f, 100.0f / 255.0f );
	ret.yellow = yellow;

	return ret;
}

const float screen_width = 2.0f;
const float width = screen_width / 128.0f;
const float step = width * 2.0f;


Handle<Node> create_lines( const Colors& colors, const Handle<Gltf>& model )
{
	auto root = model->create_node();

	// First half
	// White lines
	for ( float x = -1.0f; x < 0.0f; x += step )
	{
		// Top left
		auto a = math::Vec3( x, -1.0f, 0.0f );
		// Bottom right
		auto b = math::Vec3( x + width, 1.0f, 0.0f );

		auto line = Mesh::create_rect( a, b, colors.white );

		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}
	// + black background
	{
		auto a = math::Vec3( -1.0f, -1.0f, -0.1f );
		auto b = math::Vec3( 0.0f, 1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.black );
		
		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}

	// Second half - top
	// Yellow lines
	for ( float y = -1.0f; y < 0.0f; y += step )
	{
		auto a = math::Vec3( 0.0, y, 0.0f );
		auto b = math::Vec3( 1.0f, y + width, 0.0f );

		auto line = Mesh::create_rect( a, b, colors.yellow );
		
		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}
	// + white background
	{
		auto a = math::Vec3( 0.0f, 0.0f - width, -0.1f );
		auto b = math::Vec3( 1.0f, -1.0f, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.white );
		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}
	// Black delimitator
	{
		auto a = math::Vec3( 0.0f, 0.0f, -0.1f );
		auto b = math::Vec3( 1.0f, -width, -0.1f );
		auto line = Mesh::create_rect( a, b, colors.black );
		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}

	return root;
}


Handle<Node> create_red_lines( const Colors& colors, const Handle<Gltf>& model )
{
	auto root = model->create_node();

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

		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}
	// + black background
	{
		auto a = math::Vec3( 0.0f, -1.0f, -0.3f );
		auto b = math::Vec3( 2.0f, 1.0f, -0.3f );
		auto line = Mesh::create_rect( a, b, colors.white );
		auto node = model->create_node( std::move( line ) );
		root->add_child( node );
	}

	return root;
}


} // namespace spot::gfx


/// Draw lines
int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto gfx = Graphics();
	auto model = gfx.create_model();

	Colors colors = create_colors( model );

	auto lines = create_lines( colors , model );
	auto red_lines = create_red_lines( colors , model );

	gfx.camera.look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );
	gfx.camera.orthographic( -1.0f, 1.0, -1.0, 1.0, 0.125f, 2.0 );

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
