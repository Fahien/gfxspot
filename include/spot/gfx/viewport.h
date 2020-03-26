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



} // namespace spot::gfx
