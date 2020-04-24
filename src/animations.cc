#include "spot/gfx/animations.h"

#include <algorithm>
#include <spot/gltf/gltf.h>


namespace spot::gfx
{


void Animations::update( const float delta_time, gltf::Gltf& gltf )
{
	for ( auto& animation : gltf.animations )
	{
		animation.time.current += delta_time;
		animation.find_max_time();

		// Start from the beginning
		if ( animation.time.current >= animation.time.max )
		{
			// Reset time
			animation.time.current -= animation.time.max;
		}

		for ( size_t i = 0; i < animation.channels->size(); ++i )
		{
			auto& channel = (*animation.channels)[i];

			auto& node = channel.target.node;
			assert( node && "Channel has no target" );

			// Get times for keyframes
			std::vector<float> times = animation.get_times( channel.sampler );

			// Find the keyframe
			size_t keyframe = 1;
			for ( size_t i = 1; i < times.size(); ++i )
			{
				if ( animation.time.current > times[i] )
				{
					keyframe++;
				}
				else
				{
					break;
				}
			}

			// Start from the beginning
			if ( keyframe >= times.size() )
			{
				keyframe = 1;
			}

			if ( animation.time.current >= times[keyframe - 1] && animation.time.current <= times[keyframe] )
			{
				// Normalize time for lerp
				float norm_time =
				    ( animation.time.current - times[keyframe - 1] ) / ( times[keyframe] - times[keyframe - 1] );

				auto& values = gltf.accessors.at( channel.sampler->output );

				auto  view_index   = values.buffer_view_index;
				auto& data_view    = gltf.buffer_views.at( view_index );
				auto  data_offset  = values.byte_offset + data_view.byte_offset;
				auto  buffer_index = data_view.buffer_index;
				auto& data_buffer  = gltf.get_buffer( buffer_index );

				switch ( channel.target.path )
				{
				case spot::gltf::Animation::Target::Path::Rotation:
				{
					std::vector<math::Quat> quats = animation.get_rotations( channel.sampler );
					node->rotation = math::slerp( quats[keyframe - 1], quats[keyframe], norm_time );
					break;
				}
				case spot::gltf::Animation::Target::Path::Scale:
				{
					std::vector<math::Vec3> scales( values.count );
					std::memcpy( scales.data(), &data_buffer.data[data_offset], values.count * sizeof( math::Vec3 ) );
					node->scale = math::lerp( scales[keyframe - 1], scales[keyframe], norm_time );
					break;
				}
				case spot::gltf::Animation::Target::Path::Translation:
				{
					std::vector<math::Vec3> trans( values.count );
					std::memcpy( trans.data(), &data_buffer.data[data_offset], values.count * sizeof( math::Vec3 ) );
					node->translation = math::lerp( trans[keyframe - 1], trans[keyframe], norm_time );
					break;
				}
				case spot::gltf::Animation::Target::Path::Weights:
					break;
				default:
					assert( false && "Animation path not supported" );
					break;
				}
			}
		}
	}
}


}  // namespace spot::gfx
