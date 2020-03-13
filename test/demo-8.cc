#include <cstdlib>

#include <spot/gfx/graphics.h>

namespace spot::gfx
{

const float unit = 0.125f;

int get_red_rect_mesh( Graphics& gfx )
{
	static int mesh_index = -1;
	if ( mesh_index < 0 )
	{
		auto& material = gfx.models.create_material();
		material.ubo.color = Color( 1.0f, 0.0f, 0.0f );

		gfx.models.meshes.emplace_back(
			Mesh::create_rect(
				Vec3( -unit, -unit, 0.0f ),
				Vec3( unit, unit, 0.0f ),
				material.index
			)
		);
		mesh_index = gfx.models.meshes.size() - 1;
	}
	return mesh_index;
}

// 2x2 block
int create_tetris_rect( Graphics& gfx )
{
	auto block_index = gfx.models.create_node().index;

	// Top-left rect
	{
		auto& node = gfx.models.create_node();
		node.mesh = get_red_rect_mesh( gfx );
		node.translation.x = -unit;
		node.translation.y = -unit;

		auto block = gfx.models.get_node( block_index );
		block->children.emplace_back( node.index );
	}

	// Bottom-left rect
	{
		auto& node = gfx.models.create_node();
		node.mesh = get_red_rect_mesh( gfx );
		node.translation.x = -unit;
		node.translation.y = unit;

		auto block = gfx.models.get_node( block_index );
		block->children.emplace_back( node.index );
	}

	return block_index;
}

int create_scene( Graphics& gfx )
{
	auto scene = gfx.models.create_node().index;

	auto rect = create_tetris_rect( gfx );

	auto scene_node = gfx.models.get_node( scene );
	scene_node->children.emplace_back( rect );

	return scene;
}

}

int main()
{
	using namespace spot::gfx;

	auto gfx = Graphics();

	auto scene = create_scene( gfx );

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

		if ( gfx.render_begin() )
		{
			gfx.draw( scene );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
