#include "spot/gfx/collisions.h"
#include "spot/gltf/bounds.h"
#include "spot/gltf/node.h"

namespace spot::gfx
{


void Collisions::update( const Node& node, const math::Mat4& transform )
{
	// Current transform
	auto temp_transform = transform * node.get_matrix();

	for ( auto& child : node.children )
	{
		update( *child, temp_transform );
	}

	// Save its shape
	if ( auto bounds = node.get_bounds() )
	{
		auto& shape = bounds->get_shape();
		shape.set_matrix( temp_transform );
		boundss.emplace_back( bounds );
	}
}


void Collisions::resolve()
{
	for ( size_t i = 0; i < boundss.size(); ++i )
	{
		auto& box = boundss[i]->get_shape();

		for ( size_t j = i + 1; j < boundss.size(); ++j )
		{
			if ( !boundss[i]->dynamic && !boundss[j]->dynamic )
			{
				continue;
			}

			auto& other       = boundss[j]->get_shape();
			auto is_colliding = box.is_colliding_with( other );

			if ( box.intersects( other ) )
			{
				if ( !is_colliding )
				{
					box.add_collision( other );
					other.add_collision( box );
					if ( box.start_colliding_with )
					{
						box.start_colliding_with( other );
					}
					if ( other.start_colliding_with )
					{
						other.start_colliding_with( box );
					}
				}

				if ( box.colliding_with )
				{
					box.colliding_with( other );
				}
				if ( other.colliding_with )
				{
					other.colliding_with( box );
				}
			}
			else if ( is_colliding )
			{
				box.remove_collision( other );
				other.remove_collision( box );
				if ( box.end_colliding_with )
				{
					box.end_colliding_with( other );
				}
				if ( other.end_colliding_with )
				{
					other.end_colliding_with( box );
				}
			}
		}
	}
}


}  // namespace spot::gfx
