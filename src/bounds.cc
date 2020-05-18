#include "spot/gltf/bounds.h"
#include "spot/gltf/gltf.h"
#include "spot/gltf/node.h"


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
	return rect.intersects( r );
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


Shape& Bounds::get_shape() const
{
	switch ( type )
	{
	case Type::Rect:   return *std::get<Handle<Rect>>( shape );
	case Type::Box:    return *std::get<Handle<Box>>( shape );
	case Type::Sphere: return *std::get<Handle<Sphere>>( shape );
	default:
		assert( false && "Bounds shape type not supported" );
	}
}


std::variant<Handle<Rect>, Handle<Box>, Handle<Sphere>> Bounds::clone_shape() const
{
	switch ( type )
	{
	case Type::Rect:   return std::get<Handle<Rect>>( shape ).clone();
	case Type::Box:    return std::get<Handle<Box>>( shape ).clone();
	case Type::Sphere: return std::get<Handle<Sphere>>( shape ).clone();
	default:
		assert( false && "Bounds shape type not supported" );
	}
}



void Bounds::invalidate()
{
	Handled<Bounds>::invalidate();
	switch ( type )
	{
	case Type::Rect:   return std::get<Handle<Rect>>( shape )->invalidate();
	case Type::Box:    return std::get<Handle<Box>>( shape )->invalidate();
	case Type::Sphere: return std::get<Handle<Sphere>>( shape )->invalidate();
	default:
		assert( false && "Bounds shape type not supported" );
	}
}


} // namespace spot::gfx
