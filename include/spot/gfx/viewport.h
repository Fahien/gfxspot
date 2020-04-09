#pragma once

#include <vulkan/vulkan_core.h>

#include <spot/math/math.h>

namespace spot::gfx
{

class Window;

/// @brief Conceptual class which just stores together graphics and virtual viewport
/// @todo I am still not really sure if grouping them together is a good idea
class Viewport
{
  public:
	/// @param window Used to construct the graphics viewport
	/// @param off Offset of the virtual viewport
	/// @param ext Extent of the virtual viewport
	Viewport( const Window& window, const math::Vec2& off = { -1.0f, -1.0f }, const math::Vec2& ext = { 2.0f, 2.0f } );

	/// The graphics viewport is used by Vulkan to specify the area on which to draw
	VkViewport viewport;

	/// The virtual viewport (here called abstract since virtual is a keyword)
	/// is used to specify a different unit of reference, useful to abstract our game
	VkViewport abstract;
};


} // namespace spot::gfx
