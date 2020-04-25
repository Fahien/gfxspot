#include "spot/gltf/buffer.h"

#include <spot/file/ifstream.h>

namespace spot::gfx
{


const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


inline bool is_base64( const char c )
{
	return ( isalnum( c ) || ( c == '+' ) || ( c == '/' ) );
}


std::vector<char> base64_decode( const std::string& encoded_string )
{
	auto              in_len = encoded_string.size();
	int               i      = 0;
	int               j      = 0;
	int               in_    = 0;
	char              char_array_4[4], char_array_3[3];
	std::vector<char> ret;

	while ( in_len-- && ( encoded_string[in_] != '=' ) && is_base64( encoded_string[in_] ) )
	{
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if ( i == 4 )
		{
			for ( i = 0; i < 4; i++ )
			{
				char_array_4[i] = base64_chars.find( char_array_4[i] );
			}

			char_array_3[0] = ( char_array_4[0] << 2 ) + ( ( char_array_4[1] & 0x30 ) >> 4 );
			char_array_3[1] = ( ( char_array_4[1] & 0xf ) << 4 ) + ( ( char_array_4[2] & 0x3c ) >> 2 );
			char_array_3[2] = ( ( char_array_4[2] & 0x3 ) << 6 ) + char_array_4[3];

			for ( i = 0; ( i < 3 ); i++ )
			{
				ret.push_back( char_array_3[i] );
			}
			i = 0;
		}
	}

	if ( i )
	{
		for ( j = i; j < 4; j++ )
		{
			char_array_4[j] = 0;
		}

		for ( j = 0; j < 4; j++ )
		{
			char_array_4[j] = base64_chars.find( char_array_4[j] );
		}

		char_array_3[0] = ( char_array_4[0] << 2 ) + ( ( char_array_4[1] & 0x30 ) >> 4 );
		char_array_3[1] = ( ( char_array_4[1] & 0xf ) << 4 ) + ( ( char_array_4[2] & 0x3c ) >> 2 );
		char_array_3[2] = ( ( char_array_4[2] & 0x3 ) << 6 ) + char_array_4[3];

		for ( j = 0; ( j < i - 1 ); j++ )
		{
			ret.push_back( char_array_3[j] );
		}
	}

	return ret;
}


std::vector<char> load( const std::string& uri, const size_t byte_length )
{
	std::vector<char> data;

	// Check if it is data
	if ( uri.rfind( "data:", 0 ) == 0 )
	{
		// It is data, find the position of comma
		auto comma_pos = uri.find_first_of( ',', 5 );
		if ( comma_pos == std::string::npos )
		{
			// Error, data not good
			throw std::runtime_error{ "Data URI not valid" };
		}

		// Assume it is base64
		data = base64_decode( uri.substr( comma_pos + 1 ) );
	}
	else
	{
		auto file = file::Ifstream( uri, std::ios::binary );
		assert( file.is_open() && "Could not open the file" );
		data = file.read( byte_length );
	}

	return data;
}


ByteBuffer::ByteBuffer( std::string u, const size_t len )
: uri { std::move( u ) }
, byte_length { len }
, data { load( uri, len ) }
{
}



} // namespace
