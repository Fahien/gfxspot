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
	Viewport( const Window& window, Camera& cam );

	/// @brief Update the viewport according to the state of the window
	void update();

	/// @return The virtual viewport aspect ratio
	float get_aspect_ratio() const { return abstract.width / float( abstract.height ); }

	/// @return The graphics viewport
	VkViewport& get_viewport() { return viewport; }

	/// @return The virtual viewport
	const VkViewport& get_abstract() const { return abstract; }

	/// @brief Updates the virtual viewport
	/// reflecting changes to the associated camera
	void set_offset( float x, float y );
	void set_offset( const math::Vec2& o ) { set_offset( o.x, o.y ); }

	/// @brief Updates the virtual viewport
	/// reflecting changes to the associated camera
	void set_extent( float width, float height );
	void set_extent( const math::Vec2& e ) { set_extent( e.x, e.y ); }
	void set_extent( const VkExtent2D& e ) { set_extent( float( e.width ), float( e.height ) ); }

	/// @brief Converts window coordinates to virtual viewport coordinates
	math::Vec2 from_window( const math::Vec2& coords );

	/// @return Ratio window over viewport
	math::Vec2 win_ratio() const;

  private:
	/// Associated window
	const Window& window;

	/// The graphics viewport is used by Vulkan to specify the area on which to draw
	VkViewport viewport;

	/// Associated camera
	Camera& camera;

	/// The virtual viewport (here called abstract since virtual is a keyword)
	/// is used to specify a different unit of reference, useful to abstract our game
	VkViewport abstract;
};


} // namespace spot::gfx
