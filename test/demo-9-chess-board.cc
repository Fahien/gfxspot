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
	SolidMaterials( const Handle<Gltf>& model );

	const Handle<Material> black;
	const Handle<Material> white;
	const Handle<Material> red;
	const Handle<Material> green;
	const Handle<Material> blue;
};


SolidMaterials::SolidMaterials( const Handle<Gltf>& model )
: white { model->materials.push( Material( Color::white ) ) }
, black { model->materials.push( Material( Color::black ) ) }
, red   { model->materials.push( Material( Color::red  ) ) }
, green { model->materials.push( Material( Color::green ) ) }
, blue  { model->materials.push( Material( Color::blue  ) ) }
{}


Handle<Mesh> create_mesh( const Handle<Material>& material, const Handle<Gltf>& model )
{
	auto hs = unit / 2.0f - spacing / 2.0f;
	return model->meshes.push(
		Mesh::create_rect(
			math::Vec3( -hs, -hs, 0.0f ),
			math::Vec3( hs, hs, 0.0f ),
			material
		)
	);
}


struct SolidMeshes
{
	SolidMeshes( const Handle<Gltf>& model );

	const SolidMaterials materials;

	const Handle<Mesh> black;
	const Handle<Mesh> white;
	const Handle<Mesh> red;
	const Handle<Mesh> green;
	const Handle<Mesh> blue;
};


SolidMeshes::SolidMeshes( const Handle<Gltf>& model )
: materials { model }
, black { create_mesh( materials.black, model ) }
, white { create_mesh( materials.white, model ) }
, red { create_mesh( materials.red, model ) }
, green { create_mesh( materials.green, model ) }
, blue { create_mesh( materials.blue, model ) }
{}


// 2x2 block
Handle<Node> create_chess_board( const Handle<Gltf>& model )
{
	auto block = model->nodes.push();

	auto add_child = [&block, &model]( const Handle<Mesh>& mesh, math::Vec3 translation ) {
		auto node = model->nodes.push( Node( mesh ) );
		block->add_child( node );
		node->translation.x = translation.x;
		node->translation.y = translation.y;
		node->bounds = model->bounds.push();
		auto rect = model->rects.push( Rect( -math::Vec2::One / 2.0f, math::Vec2::One / 2.0f ) );
		rect->node = node;
		node->bounds->shape = rect;
	};

	size_t n = 16;
	for ( size_t col = 0; col < n; ++col )
	{
		uint32_t offset = col % 2;
		for ( size_t row = 0; row < n; ++row )
		{
			auto color = model->meshes.get_handle( ( row + offset ) % 2 );
			add_child( color,
				math::Vec3(
					unit / 2.0f + unit * row,
					unit / 2.0f + unit * col
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
	auto model = gfx.create_model();

	auto meshes = SolidMeshes( model );

	auto chess_board = create_chess_board( model );

	gfx.camera.look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );
	gfx.window.on_resize = {};
	gfx.viewport.set_offset( math::Vec2::Zero );
	gfx.viewport.set_extent( math::Vec2::One * 16.0f );
	gfx.camera.set_orthographic( gfx.viewport );

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
				if ( child->bounds->get_shape().contains( coords ) )
				{
					child->mesh = model->meshes.get_handle( ( child->mesh.get_index() + 1 ) % 5 );
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
