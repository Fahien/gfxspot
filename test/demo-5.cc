#include <cstdlib>
#include <filesystem>
#include <spot/math/math.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{

Handle<Node> create_line( gfx::Graphics& gfx, gfx::Dot a, gfx::Dot b )
{
	auto node = gfx.models.gltf.create_node();

	auto& mesh = gfx.models.create_mesh();
	node->mesh = mesh.index;

	auto& primitive = mesh.primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c )
	};

	primitive.indices = { 0, 1 };

	return node;
}


Handle<Node> create_lena( gfx::Graphics& gfx )
{
	return gfx.models.create_node(
		Mesh::create_quad(
			gfx.models.create_material(
				gfx.images.load( "img/lena.png" )
			).index
		)
	);
}


void rotate( Handle<Node>& n, float angle )
{
	const math::Vec3 axis = { 0.0f, 0.0f, 1.0f };
	auto rot_axis = math::Quat( axis, angle );
	n->rotation *= rot_axis;
}


} // namespace spot::gfx


int main( const int argc, const char** argv )
{
	using namespace spot;

	auto gfx = gfx::Graphics();

	Scene* scene;
	if ( argc > 1 )
	{
		auto path = std::string( argv[1] );
		scene = &gfx.models.load( path );
	}

	auto x = create_line( gfx,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 1.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	
	auto y = create_line( gfx,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 0.0f, 1.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

	auto z = create_line( gfx,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 1.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );

	auto triangle = create_lena( gfx );
	
	gfx.camera.look_at( math::Vec3::One, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.window.swipe.x != 0 )
		{
			auto angle = math::radians( gfx.window.swipe.x );
			gfx::rotate( triangle, angle );
		}

		triangle->translation.x += gfx.window.scroll.x * dt;
		triangle->translation.y += gfx.window.scroll.y * dt;

		if ( gfx.render_begin() )
		{
			gfx.draw( x );
			gfx.draw( y );
			gfx.draw( z );
			if ( scene )
			{
				gfx.draw( *scene );
			}
			else
			{
				gfx.draw( triangle );
			}

			gfx.render_end();
		}
	}

	gfx.device.wait_idle();

	return EXIT_SUCCESS;
}
