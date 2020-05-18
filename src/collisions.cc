#include "spot/gfx/collisions.h"

#include <spot/math/math.h>

#include "spot/gltf/bounds.h"
#include "spot/gltf/node.h"


namespace spot::gfx
{


void Collisions::update( Node& node )
{
	for ( auto& child : node.children )
	{
		update( *child );
	}

	// Save nodes containing bounds
	if ( auto bounds = node.get_bounds() )
	{
		auto& shape = bounds->get_shape();
		shape.set_node( node );
		nodes.emplace_back( &node );
	}
}


void Collisions::resolve()
{
	for ( size_t i = 0; i < nodes.size(); ++i )
	{
		auto first_node = nodes[i];
		auto first_bounds = first_node->get_bounds();

		for ( size_t j = i + 1; j < nodes.size(); ++j )
		{
			auto second_node = nodes[j];
			auto second_bounds = second_node->get_bounds();

			if ( !first_bounds->dynamic && !second_bounds->dynamic )
			{
				continue;
			}

			auto& first_shape = first_bounds->get_shape();
			auto& second_shape = second_bounds->get_shape();

			auto is_colliding = first_shape.is_colliding_with( second_shape );

			if ( first_shape.intersects( second_shape ) )
			{
				if ( !is_colliding )
				{
					first_shape.add_collision( second_shape );
					second_shape.add_collision( first_shape );

					if ( first_shape.start_colliding_with )
					{
						first_shape.start_colliding_with( first_shape, second_shape );
					}
					if ( second_shape.start_colliding_with )
					{
						second_shape.start_colliding_with( second_shape, first_shape );
					}
				}

				if ( first_shape.colliding_with )
				{
					first_shape.colliding_with( first_shape, second_shape );
				}
				if ( second_shape.colliding_with )
				{
					second_shape.colliding_with( second_shape, first_shape );
				}
			}
			else if ( is_colliding )
			{
				first_shape.remove_collision( second_shape );
				second_shape.remove_collision( first_shape );

				if ( first_shape.end_colliding_with )
				{
					first_shape.end_colliding_with( first_shape, second_shape );
				}
				if ( second_shape.end_colliding_with )
				{
					second_shape.end_colliding_with( second_shape, first_shape );
				}
			}
		}
	}

	// Clear everything once finished
	nodes.clear();
}


}  // namespace spot::gfx
