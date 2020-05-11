#pragma once

#include <vector>
#include <string>
#include <functional>

#include <vulkan/vulkan_core.h>
#include <spot/math/math.h>

struct GLFWwindow;


namespace spot::gfx
{


struct RequiredExtensions
{
	uint32_t count = 0;
	const char** names = nullptr;
};

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
	Window();
	~Window();

	bool is_alive();
	void update( float dt );

	math::Vec2 get_cursor_position() const;

	/// @return Window space cursor coordinates converted to viewport space coordinates
	math::Vec2 cursor_to( const VkViewport& vp ) const;

	GLFWwindow* handle = nullptr;

	VkExtent2D extent = { 512, 512 };
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
