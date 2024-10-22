#include "spot/gfx/viewport.h"
#include "spot/gfx/glfw.h"
#include "spot/gfx/camera.h"

namespace spot::gfx
{


VkViewport create_viewport( const Window& window )
{
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = window.frame.width;
	viewport.height = window.frame.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	return viewport;
}


VkViewport create_abstract( const math::Vec2& offset = { -1.0f, -1.0f }, const math::Vec2& extent = { 2.0f, 2.0f } )
{
	VkViewport abstract = {};
	abstract.x = offset.x;
	abstract.y = offset.x;
	abstract.width = extent.x;
	abstract.height = extent.y;
	abstract.minDepth = 0.125f;
	abstract.maxDepth = 16.0f;
	return abstract;
}


Viewport::Viewport( const Window& win, Camera& cam )
: window { win }
, viewport { create_viewport( window ) }
, camera { cam }
, abstract { create_abstract() }
{
	camera.update( *this );
}


void Viewport::update()
{
	viewport = create_viewport( window );
}


void Viewport::set_offset( const float x, const float y )
{
	abstract.x = x;
	abstract.y = y;

	camera.update( *this );
}


void Viewport::set_extent( const float width, const float height )
{
	abstract.width = width;
	abstract.height = height;

	camera.update( *this );
}


math::Vec2 Viewport::from_window( const math::Vec2& coords )
{
	math::Vec2 viewport_coords;

	// In window space (0,0) is up left. Increments right and down.
	viewport_coords.x = coords.x / window.extent.width * abstract.width + abstract.x;
	viewport_coords.y = ( -coords.y + window.extent.height ) / window.extent.height * abstract.height + abstract.y;

	return viewport_coords;
}


math::Vec2 Viewport::win_ratio() const
{
	math::Vec2 ratio;

	ratio.x = abstract.width / window.extent.width;
	ratio.y = abstract.height / window.extent.height;

	return ratio;
}


} // namespace spot::gfx
