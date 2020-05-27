#include "spot/gfx/image.h"

#include "spot/gfx/png.h"

namespace spot::gfx
{


Image::Image( const std::string& path )
: uri { path }
{
	// Read info without loading memory
	Png png { uri };
	width = png.width;
	height = png.height;
}


} // namespace spot::gfx
