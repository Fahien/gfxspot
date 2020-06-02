#include "spot/gltf/bounds.h"
#include "spot/gfx/model.h"
#include "spot/gfx/node.h"


namespace spot::gfx
{


void Shape::set_node( Node& n )
{
	node = &n;
	matrix = n.get_transform();
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


bool Rect::intersects( const Shape& s ) const
{
	auto rect = *this;
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;
	return s.intersects( rect );
}


bool Rect::intersects( const Rect& r ) const
{
	math::Rect rect = *this;
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;

	math::Rect other = r;
	other.a = r.matrix * other.a;
	other.b = r.matrix * other.b;

	return rect.intersects( other );
}


math::Vec2 Rect::distance( const Shape& s ) const
{
	auto rect = *this;
	/// @todo matrix * rect
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;
	return s.distance( rect );
}


math::Vec2 Rect::distance( const Rect& r ) const
{
	math::Rect rect = *this;
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;
	return rect.distance( r );
}


bool Rect::contains( const math::Vec2& p, const math::Mat4& transform ) const
{
	math::Rect rect = *this;
	rect.a = transform * rect.a;
	rect.b = transform * rect.b;
	return rect.contains( p );
}


bool Bounds::is_colliding_with( const Bounds& b ) const
{
	auto it = std::find( std::begin( collisions ), std::end( collisions ), &b );
	return it != std::end( collisions );
}


bool Bounds::intersects( const Node& a, const Node& b )
{
	math::Rect as = a.get_transform() * a.bounds->shape;
	math::Rect bs = b.get_transform() * b.bounds->shape;

	return as.intersects( bs );
}


math::Vec2 Bounds::distance( const Node& a, const Node& b )
{
	math::Rect as = a.get_transform() * a.bounds->shape;
	math::Rect bs = b.get_transform() * b.bounds->shape;

	return as.distance( bs );
}


void Bounds::add_collision( const Bounds& b )
{
	collisions.push_back( &b );
}


void Bounds::remove_collision( const Bounds& b )
{
	auto it = std::find( std::begin( collisions ), std::end( collisions ), &b );
	if ( it != std::end( collisions ) )
	{
		collisions.erase( it );
	}
}


} // namespace spot::gfx
