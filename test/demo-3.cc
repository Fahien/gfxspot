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
	auto radians = math::radians( dt * 128.0f );
	auto z = math::Vec3( 0.0f, 0.0f, 1.0f );
	auto quat = math::Quat( z, radians );
	node.rotation *= quat;

	static float acc = 0;
	acc += dt;
	node.translation.z = std::cos( math::radians( acc * 256.0f ) ) + 4.0f;
}


int32_t create_card( Graphics& gfx )
{
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

	auto card = Mesh({
		Primitive( std::move( vertices ), std::move( indices ), material.index )
	});
	auto& node = gfx.models.create_node( std::move( card ) );

	return node.index;
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto gfx = Graphics();

	auto card = create_card( gfx );

	gfx.camera.look_at( math::Vec3::Z * -2.0f, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();

		update( dt, *gfx.models.get_node( card ) );

		if ( gfx.render_begin() )
		{
			gfx.draw( card );
			gfx.render_end();
		}
	}

	gfx.device.wait_idle();

	return EXIT_SUCCESS;
}
