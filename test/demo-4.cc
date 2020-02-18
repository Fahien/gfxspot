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
	auto matrix = mth::Mat4( node.rotation );
	matrix.rotateZ( mth::radians( dt * 128.0f ) );
	node.rotation = mth::Quat( matrix );

	static float acc = 0;
	acc += dt;
	node.translation.z = std::cos( mth::radians( acc * 256.0f ) ) + 4.0f;
}


gtf::Node& create_card( gfx::Graphics& graphics )
{
	using namespace gfx;

	Mesh& card = graphics.models.meshes.emplace_back();

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

	card.primitives.emplace_back( std::move( primitive ) );

	auto& material = graphics.models.materials.emplace_back( Material() );
	material.texture = graphics.images.load( "img/card.png" );
	card.primitives[0].material = &material;

	auto& node = graphics.models.nodes.emplace_back();
	node.index = 0;
	node.mesh_index = 0;

	graphics.renderer.add( node );

	graphics.models.scene.nodes.emplace_back( node.index );

	return node;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto& scene = graphics.models.load( "img/milktruck/CesiumMilkTruck.gltf" );

	for ( auto& node : graphics.models.nodes )
	{
		graphics.renderer.add( node );
	}

	mth::Vec3 eye = {};
	mth::Vec3 origin = {};
	mth::Vec3 up = { 0.0f, 1.0f, 0.0f };

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		if ( graphics.window.scroll.y > 0 )
		{
			eye.x -= dt * 400.0f;
			eye.y -= dt * 400.0f;
			eye.z += dt * 400.0f;
			graphics.view = look_at(
				eye, origin, up
			);
		}
		else if ( graphics.window.scroll.y < 0 )
		{
			eye.x += dt * 400.0f;
			eye.y += dt * 400.0f;
			eye.z -= dt * 400.0f;
			graphics.view = look_at(
				eye, origin, up
			);
		}

		if ( graphics.render_begin() )
		{
			graphics.draw( scene );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
