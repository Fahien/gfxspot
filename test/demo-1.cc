#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace spot::gfx
{


void update( const double dt, Node& node )
{
	auto angle = -math::radians( dt * 16.0 );
	node.rotation *= math::Quat( math::Vec3::Z, angle );
}


Mesh create_lenna( Graphics& graphics )
{
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


} // namespace spot::gfx


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

	spot::math::Vec3 eye = { 1.5f, 1.5f, 1.5f };
	spot::math::Vec3 zero = {};
	spot::math::Vec3 up = { 0.0f, 1.0f, 0.0f };
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

	return EXIT_SUCCESS;
}
