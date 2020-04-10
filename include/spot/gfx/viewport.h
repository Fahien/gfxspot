#pragma once

#include <vulkan/vulkan_core.h>

#include <spot/math/math.h>

namespace spot::gfx
{

class Window;
class Camera;

/// @brief Conceptual class which just stores together graphics and virtual viewport
/// @todo I am still not really sure if grouping them together is a good idea
class Viewport
{
  public:
	/// @param window Used to construct the graphics viewport
	/// @param off Offset of the virtual viewport
	/// @param ext Extent of the virtual viewport
	Viewport( const Window& window, Camera& cam, const math::Vec2& off = { -1.0f, -1.0f }, const math::Vec2& ext = { 2.0f, 2.0f } );

	/// @return The graphics viewport
	VkViewport& get_viewport() { return viewport; }

	/// @return The virtual viewport
	const VkViewport& get_abstract() const { return abstract; }

	/// @brief Updates the virtual viewport
	/// reflecting changes to the associated camera
	void set_offset( float x, float y );

	/// @brief Updates the virtual viewport
	/// reflecting changes to the associated camera
	void set_extent( float width, float height );

  private:
	/// The graphics viewport is used by Vulkan to specify the area on which to draw
	VkViewport viewport;

	/// Associated camera
	Camera& camera;

	/// The virtual viewport (here called abstract since virtual is a keyword)
	/// is used to specify a different unit of reference, useful to abstract our game
	VkViewport abstract;
};


} // namespace spot::gfx
