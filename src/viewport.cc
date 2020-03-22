#include <spot/gfx/viewport.h>

namespace spot::gfx
{


math::Vec2 window_to_viewport( const VkExtent2D window_extent, const math::Vec2& window_coords, const Viewport& viewport )
{
	math::Vec2 viewport_coords;

	// In window space (0,0) is up left. Increments right and down.
	viewport_coords.x = window_coords.x / window_extent.width * viewport.extent.x + viewport.offset.x;
	viewport_coords.y = ( -window_coords.y + window_extent.height ) / window_extent.height * viewport.extent.y + viewport.offset.y;

	return viewport_coords;
}


} // namespace spot::gfx
