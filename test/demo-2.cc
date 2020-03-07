#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "spot/gfx/graphics.h"
#include "spot/gfx/png.h"
#include "spot/gfx/images.h"

namespace gfx = spot::gfx;


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.matrix[5] = 1.0f + std::min<float>( -std::sin( dt ) * 4.0, 0.0f );
}


gfx::Mesh create_bugart()
{
	using namespace spot::gfx;

	Mesh bugart;

	Primitive primitive;

	primitive.indices = {
		0,1,2,1,0,3,4,5,6,7,4,6,8,9,10,11,9,8,12,13,14,13,12,15,16,17,18,16,18,19,20,21,22,20,22,23
	};

	primitive.vertices.resize( 24 );

	size_t i = 0;
	primitive.vertices[i++].p = { -0.50,  0.50, -0.50 };
	primitive.vertices[i++].p = {  0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = { -0.50, -0.50,  0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = {  0.50, -0.50, -0.50 };
	primitive.vertices[i++].p = { -0.50, -0.50, -0.50 };
	primitive.vertices[i++].p = {  0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = {  0.50, -0.50,  0.50 };
	primitive.vertices[i++].p = {  0.50,  0.50, -0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50,  0.50 };
	primitive.vertices[i++].p = {  0.50, -0.50, -0.50 };
	primitive.vertices[i++].p = { -0.50, -0.50, -0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50,  0.51 }; //this
	primitive.vertices[i++].p = {  0.50, -0.50,  0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50, -0.50 };
	primitive.vertices[i++].p = { -0.50,  0.50, -0.50 };
	primitive.vertices[i++].p = {  0.00,  1.00,  0.00 };
	primitive.vertices[i++].p = {  1.00,  0.00,  0.00 };
	primitive.vertices[i++].p = {  0.00,  0.00,  1.00 };
	primitive.vertices[i++].p = { -1.00,  0.00,  0.00 };
	primitive.vertices[i++].p = {  0.00,  0.00, -1.00 };
	primitive.vertices[i++].p = {  0.00, -1.00,  0.00 };

	i = 0;
	primitive.vertices[i++].n = {  0.00,  1.00,  0.00 };
	primitive.vertices[i++].n = {  1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00,  1.00 };
	primitive.vertices[i++].n = { -1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00, -1.00 };
	primitive.vertices[i++].n = {  0.00, -1.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00,  1.00 };
	primitive.vertices[i++].n = {  0.00,  1.00,  0.00 };
	primitive.vertices[i++].n = {  1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00,  1.00 };
	primitive.vertices[i++].n = {  0.00,  1.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00, -1.00 };
	primitive.vertices[i++].n = {  0.00, -1.00,  0.00 };
	primitive.vertices[i++].n = { -1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  0.00, -1.00,  0.00 };
	primitive.vertices[i++].n = { -1.00,  0.00,  0.00 };
	primitive.vertices[i++].n = {  0.00,  0.00, -1.00 };
	primitive.vertices[i++].n = {  4.90802E-10,  0.0101,  1.00 };
	primitive.vertices[i++].n = {  0.99971,  0.00118,  0.02468 };
	primitive.vertices[i++].n = {  0.80389,  0.80387,  0.80388 };
	primitive.vertices[i++].n = {  0.51092,  0.99999,  0.00003 };
	primitive.vertices[i++].n = {  0.51092,  0.99999,  0.00003 };
	primitive.vertices[i++].n = {  4.90802E-10,  0.0101,  1.00 };

	i = 0;
	primitive.vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	primitive.vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	primitive.vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	primitive.vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	primitive.vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	primitive.vertices[i++].c = { 0.51092,  0.99999,  0.00003,  0.00004 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	primitive.vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };
	primitive.vertices[i++].c = { 0.99971,  0.00118,  0.02468,  0.00147 };
	primitive.vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };
	primitive.vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	primitive.vertices[i++].c = { 4.90802E-10,  0.0101,  1.00,  6.10531E-10 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.0025,  0.13143,  0.99939,  0.00311 };
	primitive.vertices[i++].c = { 0.80389,  0.80387,  0.80388,  1.00 };
	primitive.vertices[i++].c = { 0.99983,  0.00069,  0.02421,  0.00086 };
	primitive.vertices[i++].c = { 0.01746,  0.99574,  0.36206,  0.02171 };

	bugart.primitives.emplace_back( std::move( primitive ) );

	return bugart;
}


int main()
{
	using namespace spot::gfx;

	auto graphics = Graphics();

	auto bugart = create_bugart();

	Material material;
	material.ubo.color = { 0.8f, 0.8f, 0.8f, 1.0f };
	bugart.primitives[0].material = &material;

	graphics.models.meshes.emplace_back( std::move( bugart ) );

	auto node_index = graphics.models.create_node();
	auto node = graphics.models.get_node( node_index );
	node->mesh = 0;

	graphics.renderer.add( node_index );

	mth::Vec3 eye = { 0.0f, 0.0f, -2.0f };
	mth::Vec3 zero = {};
	mth::Vec3 up = { 0.0f, 1.0f, 0.0f };
	graphics.view = look_at( eye, zero, up );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		if ( graphics.render_begin() )
		{
			graphics.draw( node_index );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
