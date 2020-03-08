#pragma once

#include <fstream>
#include <filesystem>
#include <functional>
#include <array>

#include <vulkan/vulkan_core.h>

#include "spot/gfx/glfw.h"
#include "spot/gfx/renderer.h"
#include "spot/gfx/descriptors.h"
#include "spot/gfx/commands.h"
#include "spot/gfx/images.h"
#include "spot/gfx/models.h"
#include "spot/gfx/pipelines.h"


namespace spot::gltf
{
class Node;
class Scene;
}

namespace spot::gfx
{

struct alignas(16) Dot
{
	Dot( Vec3 pp = {}, Color cc = { 1.0f } ) : p { pp }, c { cc } {}

	Vec3  p = {};
	Color c = {};
};


struct Line
{
	Line( Dot a = {}, Dot b = {} ) : dots { a, b } {}

	std::array<Dot, 2> dots = {};

	const std::array<Index, 2> indices = { 0, 1 };

	UniformBufferObject ubo = {};
};



struct Rect
{
	Rect( Dot bottom_left, Dot top_right );

	Rect( Dot a, Dot b, Dot c, Dot d )
	: dots { a, b, c, d }
	{}

	std::array<Dot, 4> dots = {};

	const std::array<Index, 8> indices = { 0, 1, 1, 2, 2, 3, 3, 0 };

	UniformBufferObject ubo = {};
};


template<typename T>
VkVertexInputBindingDescription get_bindings();

template<typename T>
std::vector<VkVertexInputAttributeDescription> get_attributes();


struct ValidationLayers
{
	uint32_t count = 0;
	const char** names = nullptr;
};


class PhysicalDevice
{
  public:
	PhysicalDevice( VkPhysicalDevice h );

	VkSurfaceCapabilitiesKHR get_capabilities( VkSurfaceKHR s );

	/// @return Supported surface formats
	std::vector<VkSurfaceFormatKHR> get_formats( VkSurfaceKHR s );

	std::vector<VkPresentModeKHR> get_present_modes( VkSurfaceKHR s );
	uint32_t get_memory_type( uint32_t type_filter, VkMemoryPropertyFlags f );
	VkFormatProperties get_format_properties( VkFormat f ) const;

	/// @return Whether a format is supported by the GPU
	bool supports( VkFormat f ) const;

	VkPhysicalDevice handle = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties properties;
	std::vector<VkQueueFamilyProperties> queue_families;
	std::vector<VkExtensionProperties> extensions;
};


class Device;
class CommandBuffer;


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


class Queue
{
  public:
	Queue( const Device& d, uint32_t family_index, uint32_t index );

	bool supports_present( VkSurfaceKHR s );

	void submit( CommandBuffer& command_buffer, const std::vector<VkSemaphore>& waits, const std::vector<VkSemaphore>& signals = {}, Fence* fence = nullptr );

	VkResult present( const std::vector<VkSwapchainKHR>& swapchains, const std::vector<uint32_t>& image_index, const std::vector<VkSemaphore>& waits = {} );

	const Device& device;
	uint32_t family_index = 0;
	uint32_t index = 0;
	VkQueue handle = VK_NULL_HANDLE;
	VkQueueFlags flags = 0;
};


class Device
{
  public:
	Device( PhysicalDevice& p, VkSurfaceKHR s, RequiredExtensions required_extensions = {} );
	~Device();

	void wait_idle() const;
	Queue& find_queue( VkQueueFlagBits flags );
	Queue& find_graphics_queue();
	Queue& find_present_queue( VkSurfaceKHR surface );

	PhysicalDevice& physical_device;
	VkSurfaceKHR surface;
	VkDevice handle = VK_NULL_HANDLE;
	std::vector<Queue> queues;
};


class Instance
{
  public:
	Instance( RequiredExtensions req_ext = {}, ValidationLayers layers = {} );
	~Instance();

	VkInstance handle = VK_NULL_HANDLE;
	std::vector<PhysicalDevice> physical_devices;
};


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


class ShaderModule
{
  public:
	ShaderModule( Device& d, const std::filesystem::path& path );
	~ShaderModule();

	ShaderModule( ShaderModule&& );
	ShaderModule& operator=( ShaderModule&& );

	Device& device;
	VkShaderModule handle = VK_NULL_HANDLE;
};


mth::Mat4 look_at( const mth::Vec3& eye, const mth::Vec3& center, mth::Vec3 up );

class Graphics
{
  public:
	Graphics();

	bool render_begin();
	void render_end();

	void draw( Line& line );
	void draw( Rect& rect );
	void draw( Mesh& mesh );
	void draw( Primitive& prim, const math::Mat4& transform = math::Mat4::identity );

	void draw( const int node, const math::Mat4& transform = math::Mat4::identity );
	void draw( const gltf::Scene& scene );


	Glfw glfw;
	Instance instance;
	Glfw::Window window;
	Glfw::Window::Surface surface;
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

	VkViewport viewport = {};
	VkRect2D   scissor  = {};

	GraphicsPipeline line_pipeline;
	GraphicsPipeline dot_pipeline;
	GraphicsPipeline mesh_pipeline;
	GraphicsPipeline mesh_no_image_pipeline;

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

	Models models;
	Images images;

	mth::Mat4 view = mth::Mat4::identity;
	mth::Mat4 proj = mth::Mat4::identity;
};


} // namespace spot::gfx
