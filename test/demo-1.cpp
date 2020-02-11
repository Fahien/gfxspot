#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.hpp"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;


void update( const double dt, gfx::Triangle& r )
{
	r.ubo.model.rotateY( mth::radians( dt * 16.0 ) );
}


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.rotateZ( -mth::radians( dt * 16.0 ) );
}


spot::gltf::Node& create_quad( gfx::Graphics& graphics )
{
	using namespace spot::gfx;

	Mesh quad;

	Primitive primitive;

	primitive.vertices = {
		Vertex(
			Vec3( -0.5f, -0.5f, 0.0f ),
			Color( 0.3f, 0.0f, 0.0f, 0.5f ),
			Vec2( 0.0f, 0.0 ) // a
		),
		Vertex(
			Vec3( 0.5f, -0.5f, 0.0f ),
			Color( 0.0f, 0.3f, 0.0f, 0.5f ),
			Vec2( 1.0f, 0.0 ) // b
		),
		Vertex(
			Vec3( -0.5f, 0.5f, 0.0f ),
			Color( 0.3f, 0.0f, 0.3f, 0.5f ),
			Vec2( 0.0f, 1.0 ) // d
		),
		Vertex(
			Vec3( 0.5f, 0.5f, 0.0f ),
			Color( 0.0f, 0.0f, 0.3f, 0.5f ),
			Vec2( 1.0f, 1.0 ) // c
		),
	};

	// Currently, counterclockwise?
	primitive.indices = { 0, 2, 1, 1, 2, 3 };

	quad.primitives.emplace_back( std::move( primitive ) );

	auto view = graphics.images.load( "img/lena.png" );

	auto& material = graphics.models.materials.emplace_back();
	material.texture = view;
	quad.primitives[0].material = &material;

	graphics.models.meshes.emplace_back( std::move( quad ) );

	auto& node = graphics.models.nodes.emplace_back();
	node.mesh_index = 0;
	node.index = 0;
	return node;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto& quad = create_quad( graphics );

	auto square = Rect(
		Dot( Vec3( -0.5f, -0.5f ) ),
		Dot( Vec3( 0.5f, 0.5f ) ) );
	
	auto triangle = Triangle(
		Dot( Vec3( 0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( -0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 0.0f ) ) );

	graphics.renderer.add( square );
	graphics.renderer.add( triangle );
	graphics.renderer.add( quad );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update(dt, triangle.ubo);
		update(dt, square.ubo);

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
