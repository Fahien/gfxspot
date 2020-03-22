#include <cstdlib>
#include <filesystem>
#include <spot/math/math.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{

int create_line( gfx::Graphics& graphics, gfx::Dot a, gfx::Dot b )
{
	auto& node = graphics.models.create_node();

	auto& mesh = graphics.models.meshes.emplace_back();
	node.mesh = graphics.models.meshes.size() - 1;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c )
	};

	primitive.indices = { 0, 1 };

	return node.index;
}


int32_t create_lena( gfx::Graphics& graphics )
{
	auto& node = graphics.models.create_node(
		Mesh::create_quad(
			graphics.models.create_material(
				graphics.images.load( "img/lena.png" )
			).index
		)
	);

	return node.index;
}


void rotate( Node& n, float angle )
{
	const math::Vec3 axis = { 0.0f, 0.0f, 1.0f };
	auto rot_axis = math::Quat( axis, angle );
	n.rotation *= rot_axis;
}

} // namespace spot::gfx

int main( const int argc, const char** argv )
{
	using namespace spot;

	auto graphics = gfx::Graphics();

	gfx::Scene* scene;
	if ( argc > 1 )
	{
		auto path = std::string( argv[1] );
		scene = &graphics.models.load( path );
	}

	auto x = create_line( graphics,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 1.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	
	auto y = create_line( graphics,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 0.0f, 1.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

	auto z = create_line( graphics,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 1.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );

	auto triangle = create_lena( graphics );
	
	graphics.view = gfx::look_at( math::Vec3::One, math::Vec3::Zero, math::Vec3::Y );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();
		graphics.window.update( dt );

		if ( graphics.window.swipe.x != 0 )
		{
			auto angle = math::radians( graphics.window.swipe.x );
			rotate( *graphics.models.get_node( x ), angle );
			rotate( *graphics.models.get_node( y ), angle );
			rotate( *graphics.models.get_node( z ), angle );
			rotate( *graphics.models.get_node( triangle ), angle );
		}

		if ( graphics.window.scroll.y != 0 )
		{
			graphics.models.get_node( 2 )->translation.y += graphics.window.scroll.y;
		}

		if ( graphics.render_begin() )
		{
			graphics.draw( x );
			graphics.draw( y );
			graphics.draw( z );
			if ( scene )
			{
				graphics.draw( *scene );
			}
			else
			{
				graphics.draw( triangle );
			}

			graphics.render_end();
		}
	}

	graphics.device.wait_idle();

	return EXIT_SUCCESS;
}
