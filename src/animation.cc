#include "spot/gltf/animation.h"

#include <cassert>

#include "spot/gfx/model.h"
#include "spot/gltf/node.h"


namespace spot::gfx
{


std::vector<math::Quat> Animation::get_rotations( const Handle<Sampler>& sampler ) const
{
	std::vector<math::Quat> quats;

	auto len = sampler->output->count * sizeof( math::Quat );
	assert( sampler->output->get_size() == len && "Size mismatch" );

	quats.resize( sampler->output->count );
	std::memcpy( quats.data(), sampler->output->get_data(), len );

	return quats;
}


math::Quat Animation::find_last_rotation() const
{
	for ( int32_t i = channels->size() - 1; i>= 0; --i )
	{
		auto& channel = (*channels)[i];
		if ( channel.target.path == Target::Path::Rotation )
		{
			auto quats = get_rotations( channel.sampler );
			if ( quats.size() )
			{
				return quats.back();
			}
		}
	}

	return math::Quat::Identity;
}


std::vector<float> Animation::get_times( const Handle<Sampler>& sampler ) const
{
	std::vector<float> times;

	if ( sampler->input )
	{
		times.resize( sampler->input->count );
		std::memcpy( times.data(), sampler->input->get_data(), sampler->input->count * sizeof( float ) );
	}

	return times;
}


float Animation::find_max_time()
{
	for ( auto& channel : *channels )
	{
		std::vector<float> times = get_times( channel.sampler );

		auto it = std::max_element( std::begin( times ), std::end( times ) );
		if ( it != std::end( times ) )
		{
			time.max = std::max<float>( time.max, *it );
		}
	}

	return time.max;
}


void Animation::add_rotation(
	const Handle<Node>& node,
	const std::vector<float>& times,
	const std::vector<math::Quat>& quats )
{
	assert( node && "Animation should work on a valid node" );
	assert( times.size() == quats.size() && "Times and quats count does not match" );

	// Create a channel to target the node
	auto& channel = channels->emplace_back();
	channel.target.node = node;
	channel.target.path = Target::Path::Rotation;

	// Create the sampler
	channel.sampler = samplers.push();
	channel.sampler->interpolation = Sampler::Interpolation::Linear;

	// Timepoints
	{
		channel.sampler->input = model->accessors.push();
		channel.sampler->input->type = Accessor::Type::SCALAR;
		channel.sampler->input->component_type = Accessor::ComponentType::FLOAT;
		channel.sampler->input->count = times.size();

		channel.sampler->input->buffer_view = model->buffer_views.push();

		ByteBuffer buffer;
		buffer.byte_length = times.size() * sizeof( float );
		buffer.data.resize( buffer.byte_length );
		std::memcpy( buffer.data.data(), times.data(), buffer.byte_length );
		channel.sampler->input->buffer_view->buffer = model->buffers.push( std::move( buffer ) );
	}

	// Rotation values
	{
		auto accessor = model->accessors.push();
		channel.sampler->output = accessor;
		accessor->type = Accessor::Type::VEC4;
		accessor->component_type = Accessor::ComponentType::FLOAT;
		accessor->count = quats.size();

		auto buffer_view = model->buffer_views.push();
		accessor->buffer_view = buffer_view;

		auto buffer = model->buffers.push();
		buffer_view->buffer = buffer;
		buffer->byte_length = quats.size() * sizeof( math::Quat );
		buffer->data.resize( buffer->byte_length );
		std::memcpy( buffer->data.data(), quats.data(), buffer->byte_length );
	}
}


void Animation::add_rotation( const Handle<Node>& node, const float time, const math::Quat& quat )
{
	add_rotation( node, time, find_last_rotation(), quat );
}


void Animation::add_rotation( const Handle<Node>& node, const float time, const math::Quat& a, const math::Quat& b )
{
	assert( node && "Animation should work on a valid node" );

	// Timepoints
	auto max_time = find_max_time();
	assert( time > max_time && "Target time should be greater than current max time" );
	std::vector<float> times = { max_time, time };

	// Rotation values
	auto last_rotation = find_last_rotation();
	std::vector<math::Quat> quats = { a, b };

	// Create a channel to target the node
	auto& channel = channels->emplace_back();
	channel.target.node = node;
	channel.target.path = Target::Path::Rotation;

	// Create the sampler
	auto sampler = samplers.push();
	channel.sampler = sampler;
	sampler->interpolation = Sampler::Interpolation::Linear;

	{
		auto accessor = model->accessors.push();
		sampler->input = accessor;
		accessor->type = Accessor::Type::SCALAR;
		accessor->component_type = Accessor::ComponentType::FLOAT;
		accessor->count = times.size();

		auto buffer_view = model->buffer_views.push();
		accessor->buffer_view = buffer_view;

		auto buffer = model->buffers.push();
		buffer_view->buffer = buffer;
		buffer->byte_length = times.size() * sizeof( float );
		buffer->data.resize( buffer->byte_length );
		std::memcpy( buffer->data.data(), times.data(), buffer->byte_length );
	}

	{
		auto accessor = model->accessors.push();
		sampler->output = accessor;

		accessor->type = Accessor::Type::VEC4;
		accessor->component_type = Accessor::ComponentType::FLOAT;
		accessor->count = quats.size();

		auto buffer_view = model->buffer_views.push();
		accessor->buffer_view = buffer_view;

		auto buffer = model->buffers.push();
		buffer_view->buffer = buffer;
		buffer->byte_length = quats.size() * sizeof( math::Quat );
		buffer->data.resize( buffer->byte_length );
		std::memcpy( buffer->data.data(), quats.data(), buffer->byte_length );
	}
}


} // namespace spot::gfx
