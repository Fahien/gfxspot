#include "spot/gfx/hash.h"
#include "spot/gfx/graphics.h"

namespace spot::gfx
{


size_t hash( size_t h ) // base function
{
	return h;
}


size_t hash( float h ) // base function
{
	double doub = h;
	return *reinterpret_cast<size_t*>( &doub );
}


size_t hash( uint16_t h ) // base function
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
	auto h = std::hash<float>();
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
