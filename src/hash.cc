#include "spot/gfx/hash.h"
#include "spot/gfx/graphics.h"

namespace spot::gfx
{


size_t hash( size_t h )
{
	return h;
}


size_t hash( int16_t h )
{
	return hash( int64_t( h ) );
}


size_t hash( int32_t h )
{
	return hash( int64_t( h ) );
}


size_t hash( const int64_t value )
{
	auto h = std::hash<int64_t>();
	return h( value );
}


size_t hash( float value )
{
	auto h = std::hash<float>();
	return h( value );
}


size_t hash( uint16_t h )
{
	return hash( uint64_t( h ) );
}


size_t hash( uint32_t h )
{
	return hash( uint64_t( h ) );
}


size_t hash( uint64_t h )
{
	return h;
}


size_t hash( const Vec2& vec )
{
	auto h = std::hash<float>();
	return hash( vec.x, vec.y );
}


size_t hash( const Vec3& vec )
{
	auto h = std::hash<float>();
	return hash( vec.x, vec.y, vec.z );
}


size_t hash( const Color& color )
{
	return hash( color.r, color.g, color.b, color.a );
}


size_t hash( const Vertex& vert )
{
	auto hp = hash( vert.p );
	auto hc = hash( vert.c );
	auto ht = hash( vert.t );
	return hash( hp, hc, ht );
}


size_t hash( const Primitive& prim )
{
	auto hp = hash( prim.vertices );
	auto hi = hash( prim.indices );
	return hash( hp, hi );
}


} // namespace spot::gfx