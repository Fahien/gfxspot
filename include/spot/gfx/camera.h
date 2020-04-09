#pragma once

#include <spot/math/math.h>

#include "spot/gfx/viewport.h"

namespace spot::gfx
{


class Camera
{
  public:
	Camera( const Viewport& viewport = {} );

	void look_at( const math::Vec3& eye, const math::Vec3& center, math::Vec3 up );
	void orthographic( float left, float right, float bottom, float top, float near, float far );
	void orthographic( const Viewport& viewport, float near, float far );
	void perspective( const float aspect_ratio, const float y, const float far, const float near );

	math::Mat4 view;
	math::Mat4 proj;
};


} // namespace spot::gfx
