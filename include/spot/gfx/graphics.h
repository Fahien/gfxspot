#pragma once

#include <fstream>
#include <filesystem>
#include <functional>
#include <array>

#include <vulkan/vulkan_core.h>
#include <spot/gltf/gltf.h>

#include "spot/gfx/device.h"
#include "spot/gfx/glfw.h"
#include "spot/gfx/renderer.h"
#include "spot/gfx/descriptors.h"
#include "spot/gfx/commands.h"
#include "spot/gfx/images.h"
#include "spot/gfx/pipelines.h"
#include "spot/gfx/camera.h"
#include "spot/gfx/viewport.h"
#include "spot/gfx/animations.h"
#include "spot/gfx/shader.h"
#include "spot/gfx/gui.h"


namespace spot::gfx
{


struct alignas(16) Dot
{
	Dot( math::Vec3 pp = {}, Color cc = { 1.0f } ) : p { pp }, c { cc } {}

	math::Vec3  p = {};
	Color c = {};
};


struct MvpUbo
{
	math::Mat4 model = math::Mat4::Identity;
	math::Mat4 view  = math::Mat4::Identity;
	math::Mat4 proj  = math::Mat4::Identity;
};


struct LightUbo
{
	math::Vec3 position = math::Vec3::Zero;
	float _; // this is just here for alignment
	math::Vec3 color = math::Vec3::One;
};


template<typename T>
VkVertexInputBindingDescription get_bindings();


template<typename T>
std::vector<VkVertexInputAttributeDescription> get_attributes();


class Device;
class CommandBuffer;
class Fence;
class Swapchain;


class RenderPass
{
  public:
	RenderPass( Swapchain& s );
	~RenderPass();

	RenderPass& operator=( RenderPass&& o );

	Device& device;
	VkRenderPass handle = VK_NULL_HANDLE;
};


class Framebuffer
{
  public:
	Framebuffer( const std::vector<VkImageView>& view, VkExtent2D& extent, RenderPass& render_pass );
	~Framebuffer();

	Framebuffer( Framebuffer&& other );

	Device& device;
	VkFramebuffer handle = VK_NULL_HANDLE;

	VkExtent2D extent;
};


class Swapchain
{
  public:
	Swapchain( Device& d );
	~Swapchain();

	void recreate();

	Device& device;
	VkSwapchainKHR handle = VK_NULL_HANDLE;

	VkFormat format;
	VkExtent2D extent;

	std::vector<VkImage> images;
	std::vector<VkImageView> views;

  private:
	void create();

	void destroy_views();
};


class Frames
{
  public:
	Frames( Swapchain& s );

	Frames( Frames&& o ) = default;
	Frames& operator=( Frames&& o ) = default;

	std::vector<Framebuffer> create_framebuffers( RenderPass& render_pass );

	std::vector<VkImage> color_images;
	std::vector<VkImageView> color_views;

	std::vector<Image> depth_images;
	std::vector<ImageView> depth_views;
};


class Semaphore
{
  public:
	Semaphore( Device& d );
	~Semaphore();

	Semaphore( Semaphore&& other );
	Semaphore& operator=( Semaphore&& other );

	Device& device;
	VkSemaphore handle = VK_NULL_HANDLE;
};


class Fence
{
  public:
	Fence( Device& d );
	~Fence();

	Fence( Fence&& o );
	Fence& operator=( Fence&& o );

	void wait() const;
	void reset();

	Device& device;
	VkFence handle = VK_NULL_HANDLE;
	bool can_wait = true;
};


class Graphics
{
  public:
	Graphics( VkExtent2D extent = { 320, 240 } );

	bool render_begin();
	void render_end();

	void draw( const Node& node, const Mesh& mesh );
	void draw( const Node& node, const Primitive& prim );
	void draw( const Node& node );
	void draw( const Model& model );
	void draw_gui();

	Glfw glfw;
	Instance instance;
	Window window;
	Window::Surface surface;
	const char* swapchain_extension_name = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	RequiredExtensions device_required_extensions = { 1, &swapchain_extension_name };
	Device device;
	Swapchain swapchain;
	Frames frames;

	RenderPass render_pass;

	ShaderModule line_vert;
	ShaderModule line_frag;
	PipelineLayout line_layout;

	ShaderModule mesh_vert;
	ShaderModule mesh_frag;
	ShaderModule mesh_no_image_vert;
	ShaderModule mesh_no_image_frag;

	PipelineLayout mesh_layout;
	PipelineLayout mesh_no_image_layout;

	Gui gui;
	Camera camera;
	Viewport viewport;
	VkRect2D scissor = {};

	Renderer renderer;

	CommandPool command_pool;
	std::vector<CommandBuffer> command_buffers;
	CommandBuffer* current_command_buffer = nullptr;

	std::vector<Framebuffer> framebuffers;
	Framebuffer* current_framebuffer = nullptr;

	uint32_t current_frame_index = 0;

	std::vector<Semaphore> images_available;
	Semaphore* current_image_available = nullptr;

	std::vector<Semaphore> images_drawn;
	std::vector<Fence> frames_in_flight;
	Fence* current_frame_in_flight = nullptr;

	Queue& graphics_queue;
	Queue& present_queue;

	/// @brief Loads a gltf file
	/// @return A handle to the gltf model
	Handle<Model> load_model( const std::string& path );

	Animations animations;

	Uvec<Model> models;

	/// @todo Move into a scene?
	Ambient ambient = {};
	Handle<Node> light_node = {};
};


} // namespace spot::gfx
