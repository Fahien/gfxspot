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
	auto radians = mth::radians( dt * 128.0f );
	auto z = mth::Vec3( 0.0f, 0.0f, 1.0f );
	auto quat = mth::Quat( z, radians );
	node.rotation *= quat;

	static float acc = 0;
	acc += dt;
	node.translation.z = std::cos( mth::radians( acc * 256.0f ) ) + 4.0f;
}


int create_card( gfx::Graphics& graphics )
{
	using namespace gfx;

	Mesh& card = graphics.models.meshes.emplace_back();

	std::vector<Vertex> vertices = {
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
	std::vector<Index> indices = { 0, 2, 1, 1, 2, 3 };

	auto& material = graphics.models.create_material();
	material.texture = graphics.images.load( "img/card.png" );

	card.primitives.emplace_back( Primitive( std::move( vertices ), std::move( indices ), material.index ) );

	auto& node = graphics.models.create_node();
	node.mesh = 0;

	graphics.models.scene.nodes.emplace_back( node.index );

	return node.index;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto card = create_card( graphics );

	graphics.view = look_at(
		{ 0.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update( dt, *graphics.models.get_node( card ) );

		if ( graphics.render_begin() )
		{
			graphics.draw( card );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
