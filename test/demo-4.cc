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
	auto matrix = math::Mat4( node.rotation );
	matrix.rotateZ( math::radians( dt * 128.0f ) );
	node.rotation = math::Quat( matrix );

	static float acc = 0;
	acc += dt;
	node.translation.z = std::cos( math::radians( acc * 256.0f ) ) + 4.0f;
}


int create_card( gfx::Graphics& graphics )
{
	using namespace gfx;

	Mesh& card = graphics.models.meshes.emplace_back();

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

	auto& material = graphics.models.create_material();
	material.texture = graphics.images.load( "img/card.png" );

	card.primitives.emplace_back(
		Primitive(
			std::move( vertices ),
			std::move( indices ),
			material.index )
	);

	auto& node = graphics.models.gltf.scene->create_node();
	node.mesh = 0;

	return node.index;
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto graphics = Graphics();

	auto& scene = graphics.models.load( "img/milktruck/CesiumMilkTruck.gltf" );

	math::Vec3 eye = {};
	math::Vec3 origin = {};
	math::Vec3 up = { 0.0f, 1.0f, 0.0f };

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

	return EXIT_SUCCESS;
}
