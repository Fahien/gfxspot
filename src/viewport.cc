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


VkViewport create_abstract( const math::Vec2& off, const math::Vec2& ext )
{
	VkViewport abstract = {};
	abstract.x = off.x;
	abstract.y = off.y;
	abstract.width = ext.x;
	abstract.height = ext.y;
	abstract.minDepth = 0.125f;
	abstract.maxDepth = 16.0f;
	return abstract;
}


Viewport::Viewport( const Window& window, Camera& cam, const math::Vec2& off, const math::Vec2& ext )
: viewport { create_viewport( window ) }
, camera { cam }
, abstract { create_abstract( off, ext ) }
{}


void Viewport::set_offset( const float x, const float y )
{
	abstract.x = x;
	abstract.y = y;

	/// @todo Handle different types of camera
	camera.orthographic( abstract );
}


void Viewport::set_extent( const float width, const float height )
{
	abstract.width = width;
	abstract.height = height;

	/// @todo Handle different types of camera
	camera.orthographic( abstract );
}


} // namespace spot::gfx
