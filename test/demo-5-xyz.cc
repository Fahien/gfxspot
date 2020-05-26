#include <cstdlib>
#include <filesystem>
#include <spot/math/math.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{

Handle<Node> create_line( const Handle<Model>& model, gfx::Dot a, gfx::Dot b )
{
	auto node = model->nodes.push();

	auto mesh = model->meshes.push();
	node->mesh = mesh;

	auto& primitive = mesh->primitives.emplace_back();
	primitive.vertices = {
		gfx::Vertex( a.p, a.c ),
		gfx::Vertex( b.p, b.c )
	};

	primitive.indices = { 0, 1 };

	return node;
}


Handle<Node> create_lena( const Handle<Model>& model )
{
	return model->nodes.push( Node(
		model->meshes.push( Mesh::create_quad(
			model->materials.push(
				Material( model->images.load( "img/lena.png" ) )
			)
		) )
	) );
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

	Handle<gfx::Model> loaded_model;
	if ( argc > 1 )
	{
		auto path = std::string( argv[1] );
		loaded_model = gfx.load_model( path );
	}

	auto model = gfx.models.push( gfx::Model( gfx.device ) );
	auto x = create_line( model,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 1.0f, 0.0f, 0.0f ), gfx::Color( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
	
	auto y = create_line( model,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f) ),
		gfx::Dot( math::Vec3( 0.0f, 1.0f, 0.0f ), gfx::Color( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

	auto z = create_line( model,
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 0.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		gfx::Dot( math::Vec3( 0.0f, 0.0f, 1.0f ), gfx::Color( 0.0f, 0.0f, 1.0f, 1.0f ) ) );

	auto triangle = create_lena( model );
	
	gfx.camera.set_perspective( gfx.viewport, math::radians( 60.0f ) );
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
			if ( loaded_model )
			{
				gfx.draw( loaded_model );
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
