#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"
#include "spot/gfx/animations.h"

namespace spot::gfx
{


void update( const double dt, gltf::Node& node )
{
	auto matrix = math::Mat4( node.rotation );
	matrix.rotateZ( math::radians( dt * 128.0f ) );
	node.rotation = math::Quat( matrix );

	static float acc = 0;
	acc += dt;
	node.translation.z = std::cos( math::radians( acc * 256.0f ) ) + 4.0f;
}


int create_card( gfx::Graphics& gfx )
{
	using namespace gfx;

	Mesh& card = gfx.models.meshes.emplace_back();

	std::vector<Vertex> vertices = {
		Vertex(
			math::Vec3( -1.25f, -1.75f, 0.0f ),
			Color( 0.3f, 0.0f, 0.0f, 0.5f ),
			math::Vec2( 0.0f, 0.0 ) // a
		),
		Vertex(
			math::Vec3( 1.25f, -1.75f, 0.0f ),
			Color( 0.0f, 0.3f, 0.0f, 0.5f ),
			math::Vec2( 1.0f, 0.0 ) // b
		),
		Vertex(
			math::Vec3( -1.25f, 1.75f, 0.0f ),
			Color( 0.3f, 0.0f, 0.3f, 0.5f ),
			math::Vec2( 0.0f, 1.0 ) // d
		),
		Vertex(
			math::Vec3( 1.25f, 1.75f, 0.0f ),
			Color( 0.0f, 0.0f, 0.3f, 0.5f ),
			math::Vec2( 1.0f, 1.0 ) // c
		),
	};

	// Currently, counterclockwise?
	std::vector<Index> indices = { 0, 2, 1, 1, 2, 3 };

	auto& material = gfx.models.create_material();
	material.texture = gfx.images.load( "img/card.png" );

	card.primitives.emplace_back(
		Primitive(
			std::move( vertices ),
			std::move( indices ),
			material.index )
	);

	auto& node = gfx.models.gltf.scene->create_node();
	node.mesh = 0;

	return node.index;
}


} // namespace spot::gfx


int main( const int argc, const char** argv )
{
	using namespace spot::gfx;
	namespace math = spot::math;

	if ( argc < 2 )
	{
		fprintf( stderr, "Usage: %s <gltf>\n", argv[0] );
		return EXIT_FAILURE;
	}
	
	auto gfx = Graphics();

	auto path = std::string( argv[1] );
	auto& scene = gfx.models.load( path );

	auto eye = math::Vec3( 4.0f, 4.0f, 5.0f );
	gfx.camera.look_at( eye, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();
		Animations::update( dt, gfx.models.gltf );

		if ( gfx.render_begin() )
		{
			gfx.draw( scene );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}
