#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;
namespace gtf = spot::gltf;


gtf::Node& create_line( gfx::Graphics& graphics, gfx::Dot a, gfx::Dot b )
{
	auto& node = graphics.models.nodes.emplace_back();

	auto& mesh = graphics.models.meshes.emplace_back();
	node.mesh_index = graphics.models.meshes.size() - 1;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c )
	};

	primitive.indices = { 0, 1 };

	return node;
}


gtf::Node& create_triangle( gfx::Graphics& graphics, gfx::Dot a, gfx::Dot b, gfx::Dot c )
{
	auto& node = graphics.models.nodes.emplace_back();

	auto& mesh = graphics.models.meshes.emplace_back();
	node.mesh_index = graphics.models.meshes.size() - 1;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c ),
		gfx::Vertex( c.p, c.c )
	};

	primitive.indices = { 0, 1, 1, 2, 2, 0 };

	return node;
}


/// @todo Test thoroughly this, as meshes suddenly disappear at a random moment
void rotate( gtf::Node& n, float angle )
{
	auto matrix = mth::Mat4( n.rotation );
	matrix.rotateY( angle );
	n.rotation = mth::Quat( matrix );
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto& x = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 1.0f, 0.0f, 0.0f ), Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	graphics.renderer.add( x );
	
	auto& y = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		Dot( Vec3( 0.0f, 1.0f, 0.0f ), Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );
	graphics.renderer.add( y );

	auto& z = create_line( graphics,
		Dot( Vec3( 0.0f, 0.0f, 0.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 1.0f ), Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
	graphics.renderer.add( z );

	auto triangle = create_triangle( graphics,
		Dot( Vec3( 0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( -0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 0.0f ) ) );
	graphics.renderer.add( triangle );

	graphics.view = look_at(
		{ 1.5f, 1.5f, 1.5f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	);

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();
		graphics.window.update( dt );

		if ( graphics.window.swipe.x != 0 )
		{
			auto angle = mth::radians( graphics.window.swipe.x );
			rotate( x, angle );
			rotate( y, angle );
			rotate( z, angle );
			rotate( triangle, angle );
		}

		if ( graphics.render_begin() )
		{
			graphics.draw( x );
			graphics.draw( y );
			graphics.draw( z );
			graphics.draw( triangle );

			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
