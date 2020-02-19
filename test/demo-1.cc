#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;
namespace gtf = spot::gltf;


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.rotateZ( -mth::radians( dt * 16.0 ) );
}


void update( const double dt, gtf::Node& node )
{
	static mth::Mat4 model = mth::Mat4::identity;
	model.rotateZ( -mth::radians( dt * 16.0 ) );
	node.rotation = model;
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


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto square = Rect(
		Dot( Vec3( -0.5f, -0.5f ) ),
		Dot( Vec3( 0.5f, 0.5f ) ) );
	graphics.renderer.add( square );
	
	auto& triangle = create_triangle( graphics,
		Dot( Vec3( 0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( -0.5f, 0.0f, -1.0f ) ),
		Dot( Vec3( 0.0f, 0.0f, 0.0f ) ) );
	graphics.renderer.add( triangle );

	auto& quad = create_quad( graphics );
	graphics.renderer.add( quad );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update( dt, triangle );
		update( dt, square.ubo );
		update( dt, quad );

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
