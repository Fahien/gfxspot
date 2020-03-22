#include <cstdlib>

#include <spot/gfx/graphics.h>
#include <spot/gfx/viewport.h>
#include <fmt/format.h>

namespace spot::gfx
{

const float unit = 1.0f;
const Viewport viewport = Viewport( math::Vec2::zero, { unit * 16.0f, unit * 16.0f } );
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
			math::Vec3( -hs, -hs, 0.0f ),
			math::Vec3( hs, hs, 0.0f ),
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

	auto add_child = [block_index, &gfx]( uint32_t mesh, math::Vec3 translation ) {
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
				math::Vec3(
					0.0f + unit / 2.0f + unit * row,
					0.0f + unit / 2.0f + unit * col
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
	namespace math = spot::math;

	auto gfx = Graphics();
	auto meshes = SolidMeshes( gfx );

	auto chess_board = create_chess_board( gfx );

	gfx.view = look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );

	gfx.proj = ortho(
		viewport.offset.x, viewport.extent.x,
		viewport.offset.y, viewport.extent.y,
		0.125f, 2.0 );

	double tick = 1.0;
	double time = 0.0f;

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.window.click )
		{
			auto coords = window_to_viewport( gfx.window.extent, gfx.window.cursor, viewport );

			for ( auto child_index : gfx.models.get_node( chess_board )->children )
			{
				auto child = gfx.models.get_node( child_index );
				if ( child->contains( coords ) )
				{
					child->mesh = ( child->mesh + 1 ) % 5;
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
