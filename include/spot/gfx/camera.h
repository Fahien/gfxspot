#pragma once

#include <spot/math/math.h>
#include <spot/gltf/node.h>
#include <vulkan/vulkan_core.h>

namespace spot::gfx
{

class Viewport;

class Camera
{
  public:
	/// Orthographic Camera
	struct Orthographic
	{
		/// Horizontal magnification
		float xmag = 0.0f;

		/// Vertical magnification
		float ymag = 0.0f;

		/// Distance to the far clipping plane
		float zfar = 0.0f;

		/// Distance to the near clipping plane
		float znear = 0.0f;
	};

	/// Perspective camera
	struct Perspective
	{
		/// Aspect ratio of the field of view
		float aspect_ratio = 0.0f;

		/// Vertical field of view in radians
		float yfov = 0.0f;

		/// Distance to the far clipping plane
		float zfar = 0.0f;

		/// Distance to the near clipping plane
		float znear = 0.0f;
	};

	/// Type of the camera
	enum class Type
	{
		Perspective,
		Orthographic,
	};

	Camera();

	/// @brief Update the camera according to the new viewport
	void update( const Viewport& v );

	/// @return Computes and returns a view matrix
	math::Mat4 get_view() const;

	/// @return The projection matrix
	const math::Mat4& get_proj() const { return proj; }

	void look_at( const math::Vec3& eye, const math::Vec3& center, const math::Vec3& up );

	/// @brief Sets the camera as orthographic
	void set_orthographic( float xmag, float ymag, float near, float far );

	/// @param v Virtual vieport
	void set_orthographic( const Viewport& v );

	/// @brief Sets the camera as perspective
	void set_perspective( const float aspect_ratio, const float yfov, const float far, const float near );

	/// @param v Virtual vieport
	void set_perspective( const Viewport& v, const float yfov = math::radians( 60.0f ) );

	/// Eye of the camera is node's translation
	Node node = {};

  private:
	Type type = Type::Orthographic;

	/// Orthographic camera
	Orthographic orthographic;

	/// Perspective camera
	Perspective perspective;

	math::Mat4 view;
	math::Vec3 forward;
	math::Vec3 right;
	math::Vec3 up;

	math::Mat4 proj;
};


} // namespace spot::gfx
