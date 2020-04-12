#include <cstdlib>

#include <spot/gfx/graphics.h>
#include <spot/gfx/animations.h>

int main( const int argc, const char** argv )
{
	using namespace spot;
	gfx::Graphics gfx;

	auto& quad = gfx.models.create_node(
		gfx::Mesh::create_quad(
			gfx.models.create_material(
				gfx.images.load( "img/lena.png" )
			)
		)
	);

	while ( gfx.window.is_alive() )
	{
		gfx.glfw.poll();
		const auto dt = gfx.glfw.get_delta();
		gfx.window.update( dt );
		gfx::Animations::update( dt, gfx.models.gltf );

		if ( gfx.window.click.left )
		{
			// Enqueue animation rotate 180 degrees
			auto& anim = gfx.models.gltf.animations.emplace_back();

			// Create a channel to target the node
			auto& channel = anim.channels.emplace_back();
			channel.target.node = quad.index;
			channel.target.path = gltf::Animation::Target::Path::Rotation;
			channel.sampler = 0;

			// Create the sampler
			auto& sampler = anim.samplers.emplace_back();
			sampler.interpolation = gltf::Animation::Sampler::Interpolation::Linear;
			sampler.input = gfx.models.gltf.accessors.size();

			auto& times_accessor = gfx.models.gltf.accessors.emplace_back( gfx.models.gltf );
			// Create times
			// Milliseconds?
			std::vector<float> times = { 0.0f, 10.0f };
			times_accessor.count = times.size();
			times_accessor.buffer_view_index = gfx.models.gltf.buffer_views.size();

			auto& buffer_view = gfx.models.gltf.buffer_views.emplace_back();
			buffer_view.buffer_index = gfx.models.gltf.buffers.size();

			auto& buffer = gfx.models.gltf.buffers.emplace_back();
			buffer.byte_length = times.size() * sizeof( float );
			buffer.data.resize( buffer.byte_length );
			std::memcpy( buffer.data.data(), times.data(), buffer.byte_length );

			{
			sampler.output = gfx.models.gltf.accessors.size();

			auto& rot_accessor = gfx.models.gltf.accessors.emplace_back( gfx.models.gltf );
			// Create quaternions
			std::vector<math::Quat> quats( 2 );
			quats[0] = math::Quat::identity;
			quats[1] = math::Quat( math::Vec3::Z, math::radians( 180 ) );
			rot_accessor.count = quats.size();
			rot_accessor.buffer_view_index = gfx.models.gltf.buffer_views.size();

			auto& buffer_view = gfx.models.gltf.buffer_views.emplace_back();
			buffer_view.buffer_index = gfx.models.gltf.buffers.size();

			auto& buffer = gfx.models.gltf.buffers.emplace_back();
			buffer.byte_length = quats.size() * sizeof( math::Quat );
			buffer.data.resize( buffer.byte_length );
			std::memcpy( buffer.data.data(), quats.data(), buffer.byte_length );
			}
		}

		if ( gfx.render_begin() )
		{
			gfx.draw( quad.index );
			gfx.render_end();
		}
	}


	/// @todo What if we call this in Graphics::~Graphics?
	gfx.device.wait_idle();
	return EXIT_SUCCESS;
}