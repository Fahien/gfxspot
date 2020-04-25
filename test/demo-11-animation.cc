#include <cstdlib>

#include <spot/gfx/graphics.h>
#include <spot/gfx/animations.h>

int main( const int argc, const char** argv )
{
	using namespace spot;
	gfx::Graphics gfx;
	auto model = gfx.create_model();

	auto quad = model->create_node(
		gfx::Mesh::create_quad(
			model->materials.push(
				gfx::Material( model->images->load( "img/lena.png" ) )
			)
		)
	);

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );
		gfx::Animations::update( dt, model );

		if ( gfx.window.click.left )
		{
			if ( model->animations.empty() )
			{
				auto& anim = model->animations.emplace_back( *model );
				// Rotate 180 degrees
				anim.add_rotation( quad, 1.0f, math::Quat( math::Vec3::Z, math::radians( 180 ) ) );
				// Rotate another 180 degrees
				anim.add_rotation( quad, 2.0f, math::Quat( math::Vec3::Z, math::radians( 360 ) ) );
			}
			else
			{
				model->animations[0].pause = !model->animations[0].pause;
			}
			
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( quad );
			gfx.render_end();
		}
	}


	/// @todo What if we call this in Graphics::~Graphics?
	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}