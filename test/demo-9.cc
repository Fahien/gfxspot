#include <cstdlib>

#include <spot/gfx/graphics.h>
#include <spot/gfx/viewport.h>
#include <fmt/format.h>


namespace spot::gfx
{

const float unit = 1.0f;
const float spacing = unit / 16.0f;


struct SolidMaterials
{
	SolidMaterials( Graphics& gfx );

	const Handle<Material> black;
	const Handle<Material> white;
	const Handle<Material> red;
	const Handle<Material> green;
	const Handle<Material> blue;
};


SolidMaterials::SolidMaterials( Graphics& gfx )
: white { gfx.models.gltf.materials.push( Material( Color::white ) ) }
, black { gfx.models.gltf.materials.push( Material( Color::black ) ) }
, red   { gfx.models.gltf.materials.push( Material( Color::red  ) ) }
, green { gfx.models.gltf.materials.push( Material( Color::green ) ) }
, blue  { gfx.models.gltf.materials.push( Material( Color::blue  ) ) }
{}


Handle<Mesh> create_mesh( const Handle<Material>& material, Graphics& gfx )
{
	auto hs = unit / 2.0f - spacing / 2.0f;
	return gfx.models.gltf.create_mesh(
		Mesh::create_rect(
			math::Vec3( -hs, -hs, 0.0f ),
			math::Vec3( hs, hs, 0.0f ),
			material
		)
	);
}


struct SolidMeshes
{
	SolidMeshes( Graphics& gfx );

	const SolidMaterials materials;

	const Handle<Mesh> black;
	const Handle<Mesh> white;
	const Handle<Mesh> red;
	const Handle<Mesh> green;
	const Handle<Mesh> blue;
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
Handle<Node> create_chess_board( Graphics& gfx )
{
	auto block = gfx.models.gltf.create_node();

	auto add_child = [&block, &gfx]( const Handle<Mesh>& mesh, math::Vec3 translation ) {
		auto node = gfx.models.gltf.create_node( block );
		node->mesh = mesh;
		node->translation.x = translation.x;
		node->translation.y = translation.y;
	};

	size_t n = 16;
	for ( size_t col = 0; col < n; ++col )
	{
		uint32_t offset = col % 2;
		for ( size_t row = 0; row < n; ++row )
		{
			auto color = gfx.models.gltf.meshes.get_handle( ( row + offset ) % 2 );
			add_child( color,
				math::Vec3(
					0.0f + unit / 2.0f + unit * row,
					0.0f + unit / 2.0f + unit * col
				)
			);
		}
	}

	return block;
}


} // namespace spot::gfx


int main()
{
	using namespace spot::gfx;
	namespace math = spot::math;

	auto gfx = Graphics();
	auto meshes = SolidMeshes( gfx );

	auto chess_board = create_chess_board( gfx );

	gfx.viewport.set_offset( math::Vec2::Zero );
	gfx.viewport.set_extent( math::Vec2::One * 16.0f );
	gfx.camera.look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );

	double tick = 1.0;
	double time = 0.0f;

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();

		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );

		if ( gfx.window.click.left )
		{
			auto coords = gfx.viewport.from_window( gfx.window.cursor );

			for ( auto& child : chess_board->get_children() )
			{
				/// @todo Implement some sort of contains
				if ( child->contains( coords ) )
				{
					child->mesh = gfx.models.gltf.meshes.get_handle( ( child->mesh.get_index() + 1 ) % 5 );
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
