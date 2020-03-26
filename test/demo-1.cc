#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace spot::gfx
{


void update( const double dt, gltf::Node& node )
{
	auto angle = -math::radians( dt * 16.0f );
	node.rotation *= math::Quat( math::Vec3::Z, angle );
}


Mesh create_lenna( Graphics& graphics )
{
	auto view = graphics.images.load( "img/lena.png" );
	auto& material = graphics.models.create_material( view );
	return Mesh::create_quad( material.index );
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto graphics = Graphics();

	auto square = graphics.models.create_node(
		Mesh::create_rect(
			math::Vec3( -0.5f, -0.5f ),
			math::Vec3( 0.5f, 0.5f )
		)
	).index;
	
	auto triangle = graphics.models.create_node(
		Mesh::create_triangle(
			math::Vec3( 0.5f, 0.0f, -1.0f ),
			math::Vec3( -0.5f, 0.0f, -1.0f ),
			math::Vec3( 0.0f, 0.0f, 0.0f )
		)
	).index;

	auto quad = graphics.models.create_node(
		create_lenna( graphics )
	).index;

	graphics.view = look_at( math::Vec3::One * 1.5f, math::Vec3::Zero, math::Vec3::Y );

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
