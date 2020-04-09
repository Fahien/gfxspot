#include "spot/gfx/glfw.h"

#include <cassert>
#include <GLFW/glfw3.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/viewport.h"

namespace spot::gfx
{


Glfw::Glfw()
{
	glfwInit();
	glfwSwapInterval( 0 );

	required_extensions.names = glfwGetRequiredInstanceExtensions( &required_extensions.count );

	time = glfwGetTime();

	assert( glfwVulkanSupported() && "Vulkan not supported" );
}


Glfw::~Glfw()
{
	glfwTerminate();
}


void Glfw::poll()
{
	glfwPollEvents();
}


double Glfw::get_delta()
{
	auto current = glfwGetTime();
	auto delta = current - time;
	time = current;
	return delta;
}


void set_window_size( GLFWwindow* handle, const int width, const int height )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );
	window->extent.width  = width;
	window->extent.height = height;
}


void set_framebuffer_size( GLFWwindow* handle, const int width, const int height )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );
	window->frame.width  = width;
	window->frame.height = height;
}


void scroll_callback( GLFWwindow* handle, double xoffset, double yoffset )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );
	window->scroll.x = xoffset;
	window->scroll.y = yoffset;
}


/// @param button The mouse button that was pressed or released
/// @param action One of GLFW_PRESS or GLFW_RELEASE
/// @param mods Bit field describing which modifier keys were held down
void mouse_callback( GLFWwindow* handle, int button, int action, int mods )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );

	if ( button == GLFW_MOUSE_BUTTON_1 )
	{
		if ( action == GLFW_PRESS )
		{
			window->cursor = window->get_cursor_position();
			window->pressed = true;
			window->click.left = true;
		}
		else if ( action == GLFW_RELEASE )
		{
			window->cursor = window->get_cursor_position();
			window->pressed = false;
			window->swipe = {};
		}
	}

	if ( button == GLFW_MOUSE_BUTTON_2 )
	{
		if ( action == GLFW_PRESS )
		{
			window->click.right = true;
		}
	}
}


Window::Window()
{
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	handle = glfwCreateWindow( extent.width, extent.height, "Graphics", nullptr, nullptr );

	int width, height;
	glfwGetFramebufferSize( handle, &width, &height );
	frame.width = static_cast<uint32_t>( width );
	frame.height = static_cast<uint32_t>( height );

	// Callbacks
	glfwSetWindowUserPointer( handle, this );
	glfwSetWindowSizeCallback( handle, set_window_size );
	glfwSetFramebufferSizeCallback( handle, set_framebuffer_size );
	glfwSetScrollCallback( handle, scroll_callback );
	glfwSetMouseButtonCallback( handle, mouse_callback );
}


Window::~Window()
{
	glfwDestroyWindow( handle );
}


math::Vec2 Window::cursor_to( const VkViewport& viewport ) const
{
	math::Vec2 viewport_coords;

	// In window space (0,0) is up left. Increments right and down.
	viewport_coords.x = cursor.x / extent.width * viewport.width + viewport.x;
	viewport_coords.y = ( -cursor.y + extent.height ) / extent.height * viewport.height + viewport.y;

	return viewport_coords;
}


bool Window::is_alive()
{
	// Reset internal state
	scroll = {};
	click = Click();

	return !glfwWindowShouldClose( handle );
}


void Window::update( const float dt )
{
	if ( pressed )
	{
		auto current = get_cursor_position();
		swipe.x = current.x - cursor.x;
		swipe.y = current.y - cursor.y;
	}

	// Then update cursor position
	cursor = get_cursor_position();
}


math::Vec2 Window::get_cursor_position() const
{
	double x;
	double y;
	glfwGetCursorPos( handle, &x, &y );
	return { float( x ), float( y ) };
}


Window::Surface::Surface( Instance& i, Window& window )
: instance { i }
{
	auto err = glfwCreateWindowSurface( instance.handle, window.handle, nullptr, &handle );

	assert( err == VK_SUCCESS && "Cannot create Surface" );
}


Window::Surface::~Surface()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroySurfaceKHR( instance.handle, handle, nullptr );
	}
}


}
