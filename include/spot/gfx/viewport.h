#pragma once

#include <vulkan/vulkan_core.h>

#include <spot/math/math.h>

namespace spot::gfx
{

class Viewport
{
  public:
	Viewport( const math::Vec2& off = { -1.0f, -1.0f }, const math::Vec2& ext = { 2.0f, 2.0f } )
	: offset { off }, extent { ext }
	{}

	math::Vec2 offset;
	math::Vec2 extent;
};


/// @brief Transform window space coordinates to viewport space coordinates
math::Vec2 window_to_viewport( const VkExtent2D window_extent, const math::Vec2& window_coords, const Viewport& viewport );


} // namespace spot::gfx
