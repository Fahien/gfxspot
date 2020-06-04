#include "spot/gfx/collisions.h"

#include <spot/math/math.h>

#include "spot/gltf/bounds.h"
#include "spot/gfx/node.h"


namespace spot::gfx
{


void Collisions::add( Node& node )
{
	for ( auto& child : node.children )
	{
		add( *child );
	}

	// Save nodes containing bounds
	if ( node.bounds )
	{
		nodes.emplace_back( &node );
	}
}


void Collisions::update()
{
	for ( size_t i = 0; i < nodes.size(); ++i )
	{
		Node& first_node = *nodes[i];
		Bounds& first_shape = *first_node.bounds;

		for ( size_t j = i + 1; j < nodes.size(); ++j )
		{
			Node& second_node = *nodes[j];
			Bounds& second_shape = *second_node.bounds;

			if ( !first_shape.dynamic && !second_shape.dynamic )
			{
				continue;
			}

			auto is_colliding = first_shape.is_colliding_with( second_node );

			if ( Bounds::intersects( first_node, second_node ) )
			{
				if ( !is_colliding )
				{
					first_shape.add_collision( second_node );
					second_shape.add_collision( first_node );

					if ( first_shape.begin_colliding_with )
					{
						first_shape.begin_colliding_with( first_node, second_node );
					}
					if ( second_shape.begin_colliding_with )
					{
						second_shape.begin_colliding_with( second_node, first_node );
					}
				}

				if ( first_shape.colliding_with )
				{
					first_shape.colliding_with( first_node, second_node );
				}
				if ( second_shape.colliding_with )
				{
					second_shape.colliding_with( second_node, first_node );
				}
			}
			else if ( is_colliding )
			{
				first_shape.remove_collision( second_node );
				second_shape.remove_collision( first_node );

				if ( first_shape.end_colliding_with )
				{
					first_shape.end_colliding_with( first_node, second_node );
				}
				if ( second_shape.end_colliding_with )
				{
					second_shape.end_colliding_with( second_node, first_node );
				}
			}
		}
	}

	// Clear everything once finished
	nodes.clear();
}


}  // namespace spot::gfx
