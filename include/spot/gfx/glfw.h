#pragma once


#include <vector>
#include <string>

#include <vulkan/vulkan_core.h>
#include <spot/math/math.h>

namespace mth = spot::math;

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

		mth::Vec2 get_cursor_position() const;

		GLFWwindow* handle = nullptr;

		VkExtent2D extent = { 600, 600 };
		VkExtent2D frame = { 600, 600 };

		mth::Vec2 scroll = {};

		mth::Vec2 swipe = {};

		mth::Vec2 cursor = {};
		bool pressed = false;

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
