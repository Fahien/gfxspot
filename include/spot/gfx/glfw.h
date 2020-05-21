#pragma once

#include <vector>
#include <string>
#include <functional>

#include <vulkan/vulkan_core.h>
#include <spot/math/math.h>

#include "spot/gfx/vulkan_utils.h"

struct GLFWwindow;


namespace spot::gfx
{



class Instance;

class Glfw
{
  public:
	Glfw();
	~Glfw();

	void poll();
	
	double get_delta();

	RequiredExtensions required_extensions;
  private:
	double time = 0;

};


class Window
{
  public:
	Window( VkExtent2D ext = { 320, 240 } );
	~Window();

	bool is_alive();
	void update( float dt );

	math::Vec2 get_cursor_position() const;

	/// @return Window space cursor coordinates converted to viewport space coordinates
	math::Vec2 cursor_to( const VkViewport& vp ) const;

	VkExtent2D extent = { 320, 240 };

	GLFWwindow* handle = nullptr;

	VkExtent2D frame = extent;

	math::Vec2 scroll = {};

	math::Vec2 swipe = {};

	math::Vec2 cursor = {};

	struct Press
	{
		bool left = false;
		bool middle = false;
		bool right = false;

	} press;
	
	struct Click
	{
		bool left = false;
		bool middle = false;
		bool right = false;

		/// Origin of click
		math::Vec2 pos = {};
	} click;

	struct Key
	{
		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;
	} key;

	std::function<void( const VkExtent2D& )> on_resize = {};

	class Surface
	{
	  public:
		Surface( Instance& i, Window& w );
		~Surface();

		Instance& instance;
		VkSurfaceKHR handle = VK_NULL_HANDLE;
	};
};


} // namespace spot::gfx
