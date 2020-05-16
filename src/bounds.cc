#include "spot/gltf/bounds.h"
#include "spot/gltf/gltf.h"
#include "spot/gltf/node.h"


namespace spot::gfx
{


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


bool Rect::intersects( const Shape& s,
		const math::Mat4& transform ) const
{
	auto rect = *this;
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;
	return s.intersects( rect );
}


bool Rect::intersects( const Rect& r,
		const math::Mat4& transform ) const
{
	math::Rect rect = *this;
	rect.a = matrix * rect.a;
	rect.b = matrix * rect.b;
	return rect.intersects( r );
}


bool Rect::contains( const math::Vec2& p, const math::Mat4& transform ) const
{
	math::Rect rect = *this;
	rect.a = transform * rect.a;
	rect.b = transform * rect.b;
	return rect.contains( p );
}


} // namespace spot::gfx
