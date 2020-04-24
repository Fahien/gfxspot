#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace spot::gfx
{


void update( const double dt, Handle<Node>& node )
{
	auto angle = -math::radians( dt * 16.0f );
	node->rotation *= math::Quat( math::Vec3::Z, angle );
}


Mesh create_lenna( Graphics& gfx )
{
	auto view = gfx.images.load( "img/lena.png" );
	auto material = gfx.models.gltf.create_material( view );
	return Mesh::create_quad( material );
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto gfx = Graphics();

	auto square = gfx.models.gltf.create_node(
		Mesh::create_rect(
			math::Vec3( -0.5f, -0.5f ),
			math::Vec3( 0.5f, 0.5f )
		)
	);
	
	auto triangle = gfx.models.gltf.create_node(
		Mesh::create_triangle(
			math::Vec3( 0.5f, 0.0f, -1.0f ),
			math::Vec3( -0.5f, 0.0f, -1.0f ),
			math::Vec3( 0.0f, 0.0f, 0.0f )
		)
	);

	auto quad = gfx.models.gltf.create_node(
		create_lenna( gfx )
	);

	gfx.camera.look_at( math::Vec3::One * 1.5f, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();

		update( dt, triangle );
		update( dt, square );
		update( dt, quad );

		if ( gfx.render_begin() )
		{
			gfx.draw( quad );
			gfx.draw( square );
			gfx.draw( triangle );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();

	return EXIT_SUCCESS;
}
