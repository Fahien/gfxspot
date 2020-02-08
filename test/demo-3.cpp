#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.hpp"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.rotateZ( -mth::radians( dt * 16.0 ) );
}


gfx::Mesh create_quad( gfx::Graphics& graphics )
{
	using namespace gfx;

	Mesh quad;

	Primitive primitive;

	primitive.vertices = {
		Vertex(
			Vec3( -1.25f, -1.75f, 0.0f ),
			Color( 0.3f, 0.0f, 0.0f, 0.5f ),
			Vec2( 0.0f, 0.0 ) // a
		),
		Vertex(
			Vec3( 1.25f, -1.75f, 0.0f ),
			Color( 0.0f, 0.3f, 0.0f, 0.5f ),
			Vec2( 1.0f, 0.0 ) // b
		),
		Vertex(
			Vec3( -1.25f, 1.75f, 0.0f ),
			Color( 0.3f, 0.0f, 0.3f, 0.5f ),
			Vec2( 0.0f, 1.0 ) // d
		),
		Vertex(
			Vec3( 1.25f, 1.75f, 0.0f ),
			Color( 0.0f, 0.0f, 0.3f, 0.5f ),
			Vec2( 1.0f, 1.0 ) // c
		),
	};

	// Currently, counterclockwise?
	primitive.indices = { 0, 2, 1, 1, 2, 3 };

	quad.primitives.emplace_back( std::move( primitive ) );

	auto& material = graphics.models.materials.emplace_back( Material() );
	material.texture = graphics.images.load( "img/card.png" );
	quad.primitives[0].material = &material;

	return quad;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto quad = create_quad( graphics );

	graphics.renderer.add( quad );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update( dt, quad.primitives[0].ubo );

		if ( graphics.render_begin() )
		{
			graphics.draw( quad );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
