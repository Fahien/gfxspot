#include <cstdlib>

#include <spot/gfx/graphics.h>

namespace spot::gfx
{

const float unit = 0.125f;
const float spacing = unit / 16.0f;

struct SolidMaterials
{
	SolidMaterials( const Handle<Model>& model );

	Handle<Material> red;
	Handle<Material> green;
	Handle<Material> blue;
};


SolidMaterials::SolidMaterials( const Handle<Model>& model )
: red   { model->materials.push( Material( Color::Red  ) ) }
, green { model->materials.push( Material( Color::Green ) ) }
, blue  { model->materials.push( Material( Color::Blue  ) ) }
{}


Handle<Mesh> create_mesh( const Handle<Material>& material, const Handle<Model>& model )
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
	SolidMeshes( const Handle<Model>& model );

	const SolidMaterials materials;

	const Handle<Mesh> red;
	const Handle<Mesh> green;
	const Handle<Mesh> blue;
};


SolidMeshes::SolidMeshes( const Handle<Model>& model )
: materials { model }
, red { create_mesh( materials.red, model ) }
, green { create_mesh( materials.green, model ) }
, blue { create_mesh( materials.blue, model ) }
{}


// 2x2 block
Handle<Node> create_tetris_el( const Handle<Mesh>& mesh, const Handle<Model>& model )
{
	auto block = model->nodes.push();
	block->translation.y = 2.0;

	auto add_child = [&block, mesh, &model]( math::Vec3 translation ) {
		auto node = model->nodes.push( Node( mesh ) );
		block->add_child( node );
		node->translation.x = translation.x;
		node->translation.y = translation.y;
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
	auto model = gfx.models.push( gfx::Model( gfx.device ) );
	auto meshes = gfx::SolidMeshes( model );

	auto el = create_tetris_el( meshes.green, model );

	gfx.camera.look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );
	gfx.viewport.set_offset( -1.0f, gfx::unit / 2.0f );
	gfx.viewport.set_extent( 2.0f, 2.0f );
	gfx.camera.set_orthographic( gfx.viewport );
	gfx.window.on_resize = [&gfx]( const VkExtent2D& extent ) {
		gfx.viewport.set_offset( -1.0f * extent.width / extent.height );
		gfx.viewport.set_extent( 2.0f * extent.width / extent.height, 2.0f );
	};

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
			for ( auto& child : el->get_children() )
			{
				for ( auto& primitive : child->mesh->primitives )
				{
					primitive.material = meshes.materials.red;
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
