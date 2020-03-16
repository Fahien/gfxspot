#include <cstdlib>

#include <spot/gfx/graphics.h>

namespace spot::gfx
{

const float unit = 0.125f;
const float spacing = unit / 16.0f;


uint32_t create_material( const Color& c, Graphics& gfx )
{
	auto& material = gfx.models.create_material();
	material.ubo.color = c;
	return material.index;
}


struct SolidMaterials
{
	SolidMaterials( Graphics& gfx );

	const uint32_t black;
	const uint32_t white;
	const uint32_t red;
	const uint32_t green;
	const uint32_t blue;
};


SolidMaterials::SolidMaterials( Graphics& gfx )
: white { create_material( Color::white, gfx ) }
, black { create_material( Color::black, gfx ) }
, red   { create_material( Color::red,   gfx ) }
, green { create_material( Color::green, gfx ) }
, blue  { create_material( Color::blue , gfx ) }
{}


uint32_t create_mesh( uint32_t material, Graphics& gfx )
{
	auto hs = unit / 2.0f - spacing / 2.0f;
	gfx.models.meshes.emplace_back(
		Mesh::create_rect(
			Vec3( -hs, -hs, 0.0f ),
			Vec3( hs, hs, 0.0f ),
			material
		)
	);
	return gfx.models.meshes.size() - 1;
}


struct SolidMeshes
{
	SolidMeshes( Graphics& gfx );

	const SolidMaterials materials;

	const uint32_t black;
	const uint32_t white;
	const uint32_t red;
	const uint32_t green;
	const uint32_t blue;
};


SolidMeshes::SolidMeshes( Graphics& gfx )
: materials { gfx }
, black { create_mesh( materials.black, gfx ) }
, white { create_mesh( materials.white, gfx ) }
, red { create_mesh( materials.red, gfx ) }
, green { create_mesh( materials.green, gfx ) }
, blue { create_mesh( materials.blue, gfx ) }
{}


// 2x2 block
int create_chess_board( Graphics& gfx )
{
	auto& block = gfx.models.create_node();
	auto block_index = block.index;

	auto add_child = [block_index, &gfx]( uint32_t mesh, Vec3 translation ) {
		auto& node = gfx.models.create_node();
		node.mesh = mesh;
		node.translation.x = translation.x;
		node.translation.y = translation.y;

		auto block = gfx.models.get_node( block_index );
		block->children.emplace_back( node.index );
	};

	size_t n = 16;
	for ( size_t col = 0; col < n; ++col )
	{
		uint32_t offset = col % 2;
		for ( size_t row = 0; row < n; ++row )
		{
			uint32_t color = ( row + offset ) % 2;
			add_child( color,
				Vec3(
					-1.0f + unit / 2.0f + unit * row,
					0.0f + unit * col
				)
			);
		}
	}

	return block_index;
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;

	auto gfx = Graphics();
	auto meshes = SolidMeshes( gfx );

	auto chess_board = create_chess_board( gfx );

	auto eye = mth::Vec3( 0.0f, 0.0f, 1.0f ); // Out of the screen
	auto origin = mth::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = mth::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	gfx.proj = ortho( -1.0f, 1.0, 0.0 + unit / 2.0f, -2.0 + unit / 2.0f, 0.125f, 2.0 );

	double tick = 1.0;
	double time = 0.0f;

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.window.scroll.y != 0 )
		{
			/// @todo Unify gltf and gfx
			auto node = gfx.models.get_node( chess_board );
			for ( auto child : node->children )
			{
				auto child_node = gfx.models.get_node( child );
				auto& mesh = gfx.models.meshes[child_node->mesh];
				for ( auto& primitive : mesh.primitives )
				{
					primitive.set_material( ( primitive.get_material() + 1 ) % 3 );
				}
			}
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( chess_board );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
