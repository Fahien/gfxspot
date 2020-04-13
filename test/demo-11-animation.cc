#include <cstdlib>

#include <spot/gfx/graphics.h>
#include <spot/gfx/animations.h>

int main( const int argc, const char** argv )
{
	using namespace spot;
	gfx::Graphics gfx;

	auto& quad = gfx.models.create_node(
		gfx::Mesh::create_quad(
			gfx.models.create_material(
				gfx.images.load( "img/lena.png" )
			)
		)
	);

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );
		gfx::Animations::update( dt, gfx.models.gltf );

		if ( gfx.window.click.left )
		{
			auto& anim = gfx.models.gltf.animations.emplace_back( gfx.models.gltf );
			// Rotate 180 degrees
			anim.add_rotation( quad.index, 1.0f, math::Quat( math::Vec3::Z, math::radians( 180 ) ) );
			// Rotate another 180 degrees
			anim.add_rotation( quad.index, 2.0f, math::Quat( math::Vec3::Z, math::radians( 360 ) ) );
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( quad.index );
			gfx.render_end();
		}
	}


	/// @todo What if we call this in Graphics::~Graphics?
	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}