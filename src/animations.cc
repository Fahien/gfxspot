#include "spot/gfx/animations.h"

#include <algorithm>
#include <spot/gfx/model.h>
#include <spot/gltf/node.h>


namespace spot::gfx
{


void Animations::update( const float delta_time, const Model& model )
{
	if ( pause )
	{
		return;
	}

	for ( auto& animation : *model.animations )
	{
		if ( animation.state != Animation::State::Play )
		{
			continue;
		}

		animation.find_max_time();
		animation.time.current += delta_time;

		if ( animation.time.current > animation.time.max )
		{
			if ( animation.repeat )
			{
				// Reset time "smoothly"
				animation.time.current -= animation.time.max;
			}
			else
			{
				// Set current time to max and perform last animation step
				animation.time.current = animation.time.max;
				animation.state = Animation::State::Stop;
			}
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

				auto& values = channel.sampler->output;

				switch ( channel.target.path )
				{
				case spot::gfx::Animation::Target::Path::Rotation:
				{
					std::vector<math::Quat> quats = animation.get_rotations( channel.sampler );
					node->set_rotation( math::slerp( quats[keyframe - 1], quats[keyframe], norm_time ) );
					break;
				}
				case spot::gfx::Animation::Target::Path::Scale:
				{
					std::vector<math::Vec3> scales( values->count );
					std::memcpy( scales.data(), values->get_data(), values->count * sizeof( math::Vec3 ) );
					node->set_scaling( math::lerp( scales[keyframe - 1], scales[keyframe], norm_time ) );
					break;
				}
				case spot::gfx::Animation::Target::Path::Translation:
				{
					std::vector<math::Vec3> trans( values->count );
					std::memcpy( trans.data(), values->get_data(), values->count * sizeof( math::Vec3 ) );
					node->set_translation( math::lerp( trans[keyframe - 1], trans[keyframe], norm_time ) );
					break;
				}
				case spot::gfx::Animation::Target::Path::Weights:
					break;
				default:
					assert( false && "Animation path not supported" );
					break;
				}
			}
		}

		if ( animation.state == Animation::State::Stop )
		{
			// Reset timer
			animation.time.current = 0;
		}
	}
}


}  // namespace spot::gfx
