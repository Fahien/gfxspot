#ifndef GST_CAMERA_H_
#define GST_CAMERA_H_

#include <string>


namespace spot::gfx
{
/// Camera's projection.
/// A node can reference a camera to apply a transform to place the camera in the scene
struct GltfCamera
{
	/// Orthographic Camera
	struct Ortographic
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
		Ortographic,
		Perspective
	};

	/// Orthographic camera
	Ortographic orthographic;

	/// Perspective camera
	Perspective perspective;

	/// Specifies if the camera uses a perspective or orthographic projection
	Type type;

	/// Name
	std::string name = "Unknown";
};
}  // namespace spot::gfx

#endif  // GST_CAMERA_H_