#include "spot/gfx/viewport.h"

#include "spot/gfx/glfw.h"

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
	abstract.maxDepth = 2.0f;
	return abstract;
}


Viewport::Viewport( const Window& window, const math::Vec2& off, const math::Vec2& ext )
: viewport { create_viewport( window ) }
, abstract { create_abstract( off, ext ) }
{}


} // namespace spot::gfx
