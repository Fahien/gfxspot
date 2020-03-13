#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;
namespace gtf = spot::gltf;


void update( const double dt, gtf::Node& node )
{
	static mth::Mat4 model = mth::Mat4::identity;
	model.rotateZ( -mth::radians( dt * 16.0 ) );
	node.rotation = model;
}


gfx::Mesh create_lenna( gfx::Graphics& graphics )
{
	using namespace spot::gfx;

	auto& material = graphics.models.create_material();
	auto view = graphics.images.load( "img/lena.png" );
	material.texture = view;

	Mesh quad = Mesh::create_quad(
			Vec3( -0.5f, -0.5f, 0.0f ),
			Vec3( 0.5f, 0.5f, 0.0f ),
			material.index
	);

	return quad;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto square = graphics.models.create_node(
		Mesh::create_rect(
			Vec3( -0.5f, -0.5f ),
			Vec3( 0.5f, 0.5f )
		)
	).index;
	
	auto triangle = graphics.models.create_node(
		Mesh::create_triangle(
			Vec3( 0.5f, 0.0f, -1.0f ),
			Vec3( -0.5f, 0.0f, -1.0f ),
			Vec3( 0.0f, 0.0f, 0.0f )
		)
	).index;

	auto& quad = graphics.models.create_node(
		create_lenna( graphics )
	).index;

	mth::Vec3 eye = { 1.5f, 1.5f, -1.5f };
	mth::Vec3 zero = {};
	mth::Vec3 up = { 0.0f, 1.0f, 0.0f };
	graphics.view = look_at( eye, zero, up );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update( dt, *graphics.models.get_node( triangle ) );
		update( dt, *graphics.models.get_node( square ) );
		update( dt, *graphics.models.get_node( quad ) );

		if ( graphics.render_begin() )
		{
			graphics.draw( quad );
			graphics.draw( square );
			graphics.draw( triangle );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
