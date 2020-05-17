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

	// Save its shape updating the node it refers to
	if ( auto bounds = node.get_bounds() )
	{
		auto& shape = bounds->get_shape();
		shape.set_node( node );
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
						box.start_colliding_with( box, other );
					}
					if ( other.start_colliding_with )
					{
						other.start_colliding_with( other, box );
					}
				}

				if ( box.colliding_with )
				{
					box.colliding_with( box, other );
				}
				if ( other.colliding_with )
				{
					other.colliding_with( other, box );
				}
			}
			else if ( is_colliding )
			{
				box.remove_collision( other );
				other.remove_collision( box );
				if ( box.end_colliding_with )
				{
					box.end_colliding_with( box, other );
				}
				if ( other.end_colliding_with )
				{
					other.end_colliding_with( other, box );
				}
			}
		}
	}

	// Clear everything once finished
	boundss.clear();
}


}  // namespace spot::gfx
