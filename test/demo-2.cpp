#include <cstdlib>
#include <filesystem>
#include <cmath>

#include "graphics/graphics.hpp"
#include "graphics/png.h"
#include "graphics/images.h"


void update( const double dt, gfx::Triangle& r )
{
	r.ubo.model.rotateY( mth::radians( dt * 16.0 ) );
}


void update( const double dt, gfx::UniformBufferObject& ubo )
{
	ubo.model.matrix[5] = 1.0f + std::min<float>( -std::sin( dt ) * 4.0, 0.0f );
}


gfx::Mesh create_quad()
{
	using namespace gfx;

	Mesh quad;

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

	quad.primitives.emplace_back( std::move( primitive ) );

	return quad;
}


int main()
{
	using namespace gfx;

	auto graphics = Graphics();

	auto quad = create_quad();

	Material material;
	material.ubo.color = { 0.8f, 0.8f, 0.8f, 1.0f };

	std::for_each( std::begin( quad.primitives ), std::end( quad.primitives ), [&material]( auto& p ) {
		p.material = &material;
	});

	graphics.renderer.add( quad );

	while ( graphics.window.is_alive() )
	{
		graphics.glfw.poll();
		auto dt = graphics.glfw.get_delta();

		update(dt, quad.primitives[0].ubo);

		if ( graphics.render_begin() )
		{
			graphics.draw( quad );
			graphics.render_end();
		}
	}

	graphics.device.wait_idle();
	return EXIT_SUCCESS;
}
