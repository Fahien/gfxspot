#pragma once

#include <vector>

namespace spot
{

namespace gltf
{

class Node;

} // namespace gltf


namespace gfx
{

class Vec2;
class Vec3;
class Color;
class Vertex;
class Primitive;


// Base functions
size_t hash( uint16_t h );
size_t hash( size_t h );
size_t hash( float h );


size_t hash( const Vec2& vec );
size_t hash( const Vec3& vec );
size_t hash( const Color& color );
size_t hash( const Vertex& vert );
size_t hash( const Primitive& pm );


template<typename T, typename... Targs>
size_t hash( T value, Targs... args ) // recursive variadic function
{
	return hash( value ) ^ ( hash( args... ) << 1 );
}


template<typename T>
size_t hash( const std::vector<T>& vec )
{
	size_t hv = 0;

	for ( auto& elem : vec )
	{
		hv ^= ( hash( elem ) << 1 );
	}

	return hv;
}


} // namespace gfx


} // namespace spot
