#include "spot/gltf/bounds.h"
#include "spot/gltf/gltf.h"

namespace spot::gfx
{


void Shape::set_matrix( const math::Mat4& m )
{
	matrix = m;
}


bool Shape::is_colliding_with( const Shape& s ) const
{
	auto it = std::find( std::begin( collisions ), std::end( collisions ), &s );
	return it != std::end( collisions );
}


void Shape::add_collision( const Shape& s )
{
	collisions.push_back( &s );
}


void Shape::remove_collision( const Shape& s )
{
	auto it = std::find( std::begin( collisions ), std::end( collisions ), &s );
	if ( it != std::end( collisions ) )
	{
		collisions.erase( it );
	}
}


bool Rect::contains( const math::Vec2& p ) const
{
	if ( node )
	{
		math::Rect rect = *this;
		rect.offset = node->get_absolute_matrix() * rect.offset;
		return rect.contains( p );
	}
	return math::Rect::contains( p );
}


} // namespace spot::gfx
