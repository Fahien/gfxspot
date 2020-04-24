#include <cstdlib>

#include <spot/gfx/graphics.h>

#define NODE( index ) ( gfx.models.get_node( index ) )

namespace spot::gfx
{

const float unit = 0.125f;
const float spacing = unit / 16.0f;

struct SolidMaterials
{
	SolidMaterials( Graphics& gfx );

	const int32_t red;
	const int32_t green;
	const int32_t blue;
};


SolidMaterials::SolidMaterials( Graphics& gfx )
: red   { gfx.models.create_material( Color::red  ).index }
, green { gfx.models.create_material( Color::green ).index }
, blue  { gfx.models.create_material( Color::blue  ).index }
{}


int32_t create_mesh( uint32_t material, Graphics& gfx )
{
	auto hs = unit / 2.0f - spacing / 2.0f;
	return gfx.models.create_mesh(
		Mesh::create_rect(
			math::Vec3( -hs, -hs, 0.0f ),
			math::Vec3( hs, hs, 0.0f ),
			material
		)
	).index;
}


struct SolidMeshes
{
	SolidMeshes( Graphics& gfx );

	const SolidMaterials materials;

	const int32_t red;
	const int32_t green;
	const int32_t blue;
};


SolidMeshes::SolidMeshes( Graphics& gfx )
: materials { gfx }
, red { create_mesh( materials.red, gfx ) }
, green { create_mesh( materials.green, gfx ) }
, blue { create_mesh( materials.blue, gfx ) }
{}


// 2x2 block
Handle<Node> create_tetris_el( const uint32_t mesh, Graphics& gfx )
{
	auto block = gfx.models.gltf.create_node();
	block->translation.y = 2.0;

	auto add_child = [&block, mesh, &gfx]( math::Vec3 translation ) {
		auto node = gfx.models.gltf.create_node();
		node->mesh = mesh;
		node->translation.x = translation.x;
		node->translation.y = translation.y;

		block->children.emplace_back( node );
	};

	add_child( math::Vec3( 0.0f, - 2.0f * unit ) );
	add_child( math::Vec3( 0.0f, - unit ) );
	add_child( math::Vec3( 0.0f, 0.0f ) );
	add_child( math::Vec3( unit, 0.0f ) );

	return block;
}


} // namespace spot


int main()
{
	using namespace spot;

	auto gfx = gfx::Graphics();
	auto meshes = gfx::SolidMeshes( gfx );

	auto el = create_tetris_el( meshes.green, gfx );

	gfx.camera.look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );
	gfx.camera.orthographic( -1.0f, 1.0, 0.0 + gfx::unit / 2.0f, 2.0 + gfx::unit / 2.0f, 0.125f, 2.0 );

	double tick = 1.0;
	double time = 0.0f;

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		time += dt;
		if ( time >= tick )
		{
			if ( el->translation.y > 0.0f )
			{
				el->translation.y -= gfx::unit;
			}
			time = 0.0;
		}

		if ( gfx.window.click.left )
		{
			el->rotation *= math::Quat( math::Vec3::Z, math::radians( 90.0f ) );
		}

		if ( gfx.window.scroll.y != 0 )
		{
			/// @todo Unify gltf and gfx
			for ( auto& child : el->children )
			{
				auto& mesh = gfx.models.meshes[child->mesh];
				for ( auto& primitive : mesh.primitives )
				{
					primitive.set_material( ( primitive.get_material() + 1 ) % 3 );
				}
			}
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( el );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
