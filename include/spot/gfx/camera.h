#pragma once

#include <spot/math/math.h>
#include <spot/gltf/node.h>
#include <vulkan/vulkan_core.h>

namespace spot::gfx
{


class Camera
{
  public:
	Camera();

	/// @return Computes and returns a view matrix
	math::Mat4 get_view() const;

	/// @return The projection matrix
	const math::Mat4& get_proj() const { return proj; }

	void look_at( const math::Vec3& eye, const math::Vec3& center, const math::Vec3& up );
	void orthographic( float left, float right, float bottom, float top, float near, float far );
	void orthographic( const VkViewport& viewport );
	void perspective( const float aspect_ratio, const float y, const float far, const float near );

	/// Eye of the camera is node's translation
	Node node;

  private:
	math::Mat4 view;
	math::Vec3 forward;
	math::Vec3 right;
	math::Vec3 up;

	math::Mat4 proj;
};


} // namespace spot::gfx
