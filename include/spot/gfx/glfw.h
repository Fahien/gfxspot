#pragma once

#include <vector>
#include <string>

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

	class Window
	{
	  public:
		Window();
		~Window();

		bool is_alive();
		void update( float dt );

		math::Vec2 get_cursor_position() const;

		GLFWwindow* handle = nullptr;

		VkExtent2D extent = { 512, 512 };
		VkExtent2D frame = extent;

		math::Vec2 scroll = {};

		math::Vec2 swipe = {};

		math::Vec2 cursor = {};
		bool pressed = false;
		bool click = false;

		class Surface
		{
		  public:
			Surface( Instance& i, Window& w );
			~Surface();

			Instance& instance;
			VkSurfaceKHR handle = VK_NULL_HANDLE;
		};
	};

  private:
	double time = 0;

};



}
