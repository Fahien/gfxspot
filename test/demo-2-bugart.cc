#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"


namespace spot::gfx
{

void update( const double dt, gfx::MvpUbo& ubo )
{
	ubo.model.matrix[5] = 1.0f + std::min<float>( -std::sin( dt ) * 4.0, 0.0f );
}


Handle<Mesh> create_bugart( const Handle<Model>& model )
{
	using namespace spot;

	auto bugart = model->meshes.push();

	std::vector<Index> indices;

	indices = {
		0,1,2,1,0,3,4,5,6,7,4,6,8,9,10,11,9,8,12,13,14,13,12,15,16,17,18,16,18,19,20,21,22,20,22,23
	};

	std::vector<Vertex> vertices;
	vertices.resize( 24 );

	size_t i = 0;
	vertices[i++].p = { -0.50,  0.50, -0.50 };
	vertices[i++].p = {  0.50,  0.50,  0.50 };
	vertices[i++].p = { -0.50, -0.50,  0.50 };
	vertices[i++].p = { -0.50,  0.50,  0.50 };
	vertices[i++].p = {  0.50, -0.50, -0.50 };
	vertices[i++].p = { -0.50, -0.50, -0.50 };
	vertices[i++].p = {  0.50,  0.50,  0.50 };
	vertices[i++].p = { -0.50,  0.50,  0.50 };
	vertices[i++].p = {  0.50, -0.50,  0.50 };
	vertices[i++].p = {  0.50,  0.50, -0.50 };
	vertices[i++].p = { -0.50,  0.50,  0.50 };
	vertices[i++].p = { -0.50,  0.50,  0.50 };
	vertices[i++].p = {  0.50, -0.50, -0.50 };
	vertices[i++].p = { -0.50, -0.50, -0.50 };
	vertices[i++].p = { -0.50,  0.50,  0.51 }; //this
	vertices[i++].p = {  0.50, -0.50,  0.50 };
	vertices[i++].p = { -0.50,  0.50, -0.50 };
	vertices[i++].p = { -0.50,  0.50, -0.50 };
	vertices[i++].p = {  0.00,  1.00,  0.00 };
	vertices[i++].p = {  1.00,  0.00,  0.00 };
	vertices[i++].p = {  0.00,  0.00,  1.00 };
	vertices[i++].p = { -1.00,  0.00,  0.00 };
	vertices[i++].p = {  0.00,  0.00, -1.00 };
	vertices[i++].p = {  0.00, -1.00,  0.00 };

	i = 0;
	vertices[i++].n = {  0.00,  1.00,  0.00 };
	vertices[i++].n = {  1.00,  0.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00,  1.00 };
	vertices[i++].n = { -1.00,  0.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00, -1.00 };
	vertices[i++].n = {  0.00, -1.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00,  1.00 };
	vertices[i++].n = {  0.00,  1.00,  0.00 };
	vertices[i++].n = {  1.00,  0.00,  0.00 };
	vertices[i++].n = {  1.00,  0.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00,  1.00 };
	vertices[i++].n = {  0.00,  1.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00, -1.00 };
	vertices[i++].n = {  0.00, -1.00,  0.00 };
	vertices[i++].n = { -1.00,  0.00,  0.00 };
	vertices[i++].n = {  0.00, -1.00,  0.00 };
	vertices[i++].n = { -1.00,  0.00,  0.00 };
	vertices[i++].n = {  0.00,  0.00, -1.00 };
	vertices[i++].n = {  4.90802E-10,  0.0101,  1.00 };
	vertices[i++].n = {  0.99971,  0.00118,  0.02468 };
	vertices[i++].n = {  0.80389,  0.80387,  0.80388 };
	vertices[i++].n = {  0.51092,  0.99999,  0.00003 };
	vertices[i++].n = {  0.51092,  0.99999,  0.00003 };
	vertices[i++].n = {  4.90802E-10,  0.0101,  1.00 };

	i = 0;
	vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };
	vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };
	vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };

	auto material = model->materials.push(
		Material( Color { 0.8f, 0.8f, 0.8f, 1.0f } )
	);

	bugart->primitives.emplace_back(
		Primitive(
			std::move( vertices ),
			std::move( indices ),
			material
		)
	);

	return bugart;
}

} // namespace spot::gfx


int main()
{
	using namespace spot;

	auto gfx = gfx::Graphics();
	auto model = gfx.models.push( gfx::Model( gfx.device ) );

	auto bugart = create_bugart( model );

	auto node = model->nodes.push();
	node->mesh = bugart;

	gfx.camera.look_at( math::Vec3::Z * -2.0f, math::Vec3::Zero, math::Vec3::Y );

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		auto dt = gfx.glfw.get_delta();

		if ( gfx.render_begin() )
		{
			gfx.draw( *node );
			gfx.render_end();
		}
	}

	return EXIT_SUCCESS;
}
