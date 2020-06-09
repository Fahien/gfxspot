#include "spot/gfx/glfw.h"

#include <cassert>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "spot/gfx/graphics.h"
#include "spot/gfx/viewport.h"

#define CHECK_KEY( K, k ) if ( key == GLFW_KEY_##K ) { \
	if ( action == GLFW_PRESS ) { window->input.key.down.k = true; window->input.key.value.k = true; } \
	else if ( action == GLFW_RELEASE ) { window->input.key.value.k = false; window->input.key.up.k = true; } }


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
	auto delta = std::min( current - time, 0.125 );
	time = current;
	return delta;
}


void set_window_size( GLFWwindow* handle, const int width, const int height )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );
	window->extent.width  = static_cast<uint32_t>( width );
	window->extent.height = static_cast<uint32_t>( height );
	if ( window->on_resize )
	{
		window->on_resize( window->extent );
	}
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
	window->input.scroll.x = xoffset;
	window->input.scroll.y = yoffset;
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
			window->input.click.pos = window->get_cursor_position();
			window->input.press.left = true;
			window->input.click.left = true;
		}
		else if ( action == GLFW_RELEASE )
		{
			window->input.press.left = false;
			window->input.swipe = {};
		}
	}

	if ( button == GLFW_MOUSE_BUTTON_2 )
	{
		if ( action == GLFW_PRESS )
		{
			window->input.click.pos = window->get_cursor_position();
			window->input.press.right = true;
			window->input.click.right = true;
		}
		else if ( action == GLFW_RELEASE )
		{
			window->input.press.right = false;
			window->input.swipe = {};
		}
	}

	if ( button == GLFW_MOUSE_BUTTON_3 )
	{
		if ( action == GLFW_PRESS )
		{
			window->input.click.pos = window->get_cursor_position();
			window->input.press.middle = true;
			window->input.click.middle = true;
		}
		else if ( action == GLFW_RELEASE )
		{
			window->input.press.middle = false;
			window->input.swipe = {};
		}
	}
}


/// @param key The keyboard key that was pressed or released
/// @param scancode The system-specific scancode of the key
/// @param action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`
/// @param mods Bit field describing which modifier keys were held down
void key_callback( GLFWwindow* handle, int key, int scancode, int action, int mods )
{
	auto window = reinterpret_cast<Window*>( glfwGetWindowUserPointer( handle ) );

	auto& io = ImGui::GetIO();
	if ( action == GLFW_PRESS )
	{
		io.KeysDown[key] = true;
	}
	if ( action == GLFW_RELEASE )
	{
		io.KeysDown[key] = false;
	}

	CHECK_KEY( W, w );
	CHECK_KEY( A, a );
	CHECK_KEY( S, s );
	CHECK_KEY( D, d );
	CHECK_KEY( R, r );
	CHECK_KEY( ENTER, enter );
}


Window::Window( VkExtent2D ext )
: extent { ext }
{
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	handle = glfwCreateWindow( extent.width, extent.height, "Graphics", nullptr, nullptr );

	int width, height;
	glfwGetFramebufferSize( handle, &width, &height );
	frame.width = uint32_t( width );
	frame.height = uint32_t( height );

	// Callbacks
	glfwSetWindowUserPointer( handle, this );
	glfwSetWindowSizeCallback( handle, set_window_size );
	glfwSetFramebufferSizeCallback( handle, set_framebuffer_size );
	glfwSetScrollCallback( handle, scroll_callback );
	glfwSetMouseButtonCallback( handle, mouse_callback );
	glfwSetKeyCallback( handle, key_callback );
}


Window::~Window()
{
	glfwDestroyWindow( handle );
}


void Window::should_close()
{
	glfwSetWindowShouldClose( handle, GLFW_TRUE );
}


math::Vec2 Window::cursor_to( const VkViewport& viewport ) const
{
	math::Vec2 viewport_coords;

	// In window space (0,0) is up left. Increments right and down.
	viewport_coords.x = input.cursor.x / extent.width * viewport.width + viewport.x;
	viewport_coords.y = ( -input.cursor.y + extent.height ) / extent.height * viewport.height + viewport.y;

	return viewport_coords;
}


bool Window::is_alive()
{
	// Reset internal state
	input.scroll = {};
	input.swipe = {};
	input.click = Input::Click();

	input.key.down = Input::Key();
	input.key.up = Input::Key();

	return !glfwWindowShouldClose( handle );
}


void Window::update()
{
	auto current = get_cursor_position();

	if ( input.press.left || input.press.middle || input.press.right )
	{
		input.swipe.x = current.x - input.cursor.x;
		input.swipe.y = -(current.y - input.cursor.y);
	}

	// Then update cursor position
	input.cursor = get_cursor_position();
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


} // namespace spot::gfx
