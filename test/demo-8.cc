#include <cstdlib>

#include <spot/gfx/graphics.h>

namespace spot::gfx
{

const float unit = 0.125f;
const float spacing = unit / 16.0f;


int64_t get_red_rect_mesh( Graphics& gfx )
{
	static int64_t mesh_index = -1;
	if ( mesh_index < 0 )
	{
		auto& material = gfx.models.create_material();
		material.ubo.color = Color( 1.0f, 0.0f, 0.0f );

		gfx.models.meshes.emplace_back(
			Mesh::create_rect(
				Vec3( -unit / 2.0f, -unit / 2.0f, 0.0f ),
				Vec3( unit / 2.0f, unit / 2.0f, 0.0f ),
				material.index
			)
		);
		mesh_index = gfx.models.meshes.size() - 1;
	}
	return mesh_index;
}

// 2x2 block
int create_tetris_el( Graphics& gfx )
{
	auto block_index = gfx.models.create_node().index;

	auto add_child = [block_index, &gfx]( Vec3 translation ) {
		auto& node = gfx.models.create_node();
		node.mesh = get_red_rect_mesh( gfx );
		node.translation.x = translation.x;
		node.translation.y = translation.y;

		auto block = gfx.models.get_node( block_index );
		block->children.emplace_back( node.index );
	};

	auto hs = spacing / 2.0f;
	add_child( Vec3( 0.0f, 2.0f * ( - unit - hs ) ) );
	add_child( Vec3( 0.0f, -unit - hs ) );
	add_child( Vec3( 0.0f, 0.0f ) );
	add_child( Vec3( unit + hs, 0.0f ) );

	return block_index;
}

} // namespace spot::gfx

int main()
{
	using namespace spot::gfx;

	auto gfx = Graphics();

	auto el = create_tetris_el( gfx );

	auto eye = mth::Vec3( 0.0f, 0.0f, 1.0f ); // Out of the screen
	auto origin = mth::Vec3( 0.0f, 0.0f, 0.0f ); // Look at origin
	auto up = mth::Vec3( 0.0f, 1.0f, 0.0f ); // Up is the sky
	gfx.view = look_at( eye, origin, up );

	gfx.proj = ortho( -1.0f, 1.0, -1.0, 1.0, 0.125f, 2.0 );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.window.click )
		{
			auto node = gfx.models.get_node( el );
			auto z = spot::math::Vec3( 0.0f, 0.0f, 1.0f );
			node->rotation *= spot::math::Quat( z, spot::math::radians( 90.0f ) );
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( el );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
