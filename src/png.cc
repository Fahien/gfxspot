#include "spot/gfx/png.h"

#include <cassert>
#include <png.h>
#include <cstdio>
#include <cstdlib>
#include <spot/log.h>

#include "spot/gfx/graphics.h"

namespace spot::gfx
{


void handle_error( png_struct* png, const char* msg )
{
	loge( "PNG error: {}\n", msg );
	exit( EXIT_FAILURE );
}


void handle_warning( png_struct* png, const char* msg )
{
	loge( "PNG warning: {}\n", msg );
	exit( EXIT_FAILURE );
}


void read_status( png_struct* png, uint32_t row, int pass )
{
	Png* obj = ( Png* )png_get_error_ptr( png );
	
	uint32_t percent = row * 100.0f / obj->height;
	std::printf("\r");
	logi( "Read\t{}%", percent );
	std::fflush( stdout );
}


const char* color_type_to_string( int color_type )
{
	switch ( color_type )
	{
	case PNG_COLOR_TYPE_GA: return "Gray Alpha";
	case PNG_COLOR_TYPE_GRAY: return "Gray";
	case PNG_COLOR_TYPE_PALETTE: return "Palette";
	case PNG_COLOR_TYPE_RGB: return "RGB";
	case PNG_COLOR_TYPE_RGBA: return "RGBA";
	default: return "UNKNOWN";
	}
}

void Png::print_info()
{
	logi( "Size\t{}x{}\n", width, height );
	logi( "Depth\t{}\n", bit_depth );
	logi( "Color\t{}\n", color_type_to_string( color_type ) );
	logi( "Chans\t{}\n", channels );
}


Png::Png( const std::string& path )
{
	png = png_create_read_struct( PNG_LIBPNG_VER_STRING, this, handle_error, handle_warning );
	assert( png && "Cannot create PNG read" );

	info = png_create_info_struct( png );
	assert( info && "Cannot create PNG info" );
	
	end = png_create_info_struct( png );
	assert( end && "Cannot create PNG end info" );

	auto res = setjmp( png_jmpbuf( png ) );
	assert( !res && "Cannot set PNG jump" );

	file = std::fopen( path.c_str(), "rb" );
	assert( file && "Cannot open png file" );

	png_init_io( png, file );

	// Meter
	png_set_read_status_fn( png, read_status );

	png_read_info( png, info );

	png_get_IHDR( png, info, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method );

	/// Some graphics card only support sampling from RGBA, therefore we are forcing RGBA
	/// @todo Improve by querying capabilities of the GPU before adjusting png info
	if ( color_type == PNG_COLOR_TYPE_PALETTE )
	{
		png_set_palette_to_rgb( png );
		png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
		png_read_update_info( png, info );
		color_type = png_get_color_type( png, info );
	}
	else if ( color_type == PNG_COLOR_TYPE_RGB )
	{
		png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
		png_read_update_info( png, info );
		color_type = png_get_color_type( png, info );
	}

	/// @todo Support different bit depths
	assert( bit_depth == 8 && "PNG bit depth not supported" );
	channels = png_get_channels( png, info );
	print_info();
}


void read_data( png_structp png_ptr, png_bytep dst, png_size_t length )
{
	png_voidp io_ptr = png_get_io_ptr( png_ptr );
	assert( io_ptr && "Invalid png io_ptr" );
	auto mem = reinterpret_cast<std::vector<uint8_t>*>( io_ptr );

	std::memcpy( dst, mem->data(), length );
	mem->erase( std::begin( *mem ), std::begin( *mem ) + length );
}


Png::Png( std::vector<uint8_t>& mem )
{
	png = png_create_read_struct( PNG_LIBPNG_VER_STRING, this, handle_error, handle_warning );
	assert( png && "Cannot create PNG read" );

	info = png_create_info_struct( png );
	assert( info && "Cannot create PNG info" );
	
	end = png_create_info_struct( png );
	assert( end && "Cannot create PNG end info" );

	auto res = setjmp( png_jmpbuf( png ) );
	assert( !res && "Cannot set PNG jump" );

	assert( mem.size() && "Cannot load png from empy memory" );

	png_set_read_fn( png, &mem, read_data );

	// Meter
	png_set_read_status_fn( png, read_status );

	png_read_info( png, info );

	png_get_IHDR( png, info, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method );

	/// Some graphics card only support sampling from RGBA, therefore we are forcing RGBA
	/// @todo Improve by querying capabilities of the GPU before adjusting png info
	if ( color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY )
	{
		if ( color_type == PNG_COLOR_TYPE_PALETTE )
		{
			png_set_palette_to_rgb( png );
		}
		else if ( color_type == PNG_COLOR_TYPE_GRAY )
		{
			png_set_gray_to_rgb( png );
		}
		png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
		png_read_update_info( png, info );
		color_type = png_get_color_type( png, info );
	}
	else if ( color_type == PNG_COLOR_TYPE_RGB )
	{
		png_set_add_alpha( png, 255, PNG_FILLER_AFTER );
		png_read_update_info( png, info );
		color_type = png_get_color_type( png, info );
	}

	assert( bit_depth == 8 && "PNG bit depth not supported" );
	channels = png_get_channels( png, info );
	print_info();
}


size_t Png::get_size() const
{
	return width * height * channels;
}


void Png::load( png_byte* bytes )
{
	assert( bytes && "Cannot load png data into a nullptr" );

	// Allocate memory to store pointers to rows
	rows.resize( height );
	for( uint32_t i = 0; i < height; ++i )
	{
		size_t offset = width * channels * i;
		rows[i] = bytes + offset;
	}

	png_read_image( png, rows.data() );
	png_read_end( png, info );
	std::printf("\n");
}

Png::~Png()
{
	png_destroy_read_struct( &png, &info, &end );
	std::fclose( file );
}

} // namespace spot::gfx
