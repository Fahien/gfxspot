#include "spot/gfx/graphics.hpp"

#include <array>
#include <filesystem>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cmath>

#include <spot/gltf/node.h>

namespace gtf = spot::gltf;


namespace spot::gfx
{


Rect::Rect( Dot bottom_left, Dot top_right )
: dots {
		bottom_left,
		{ { top_right.p.x, bottom_left.p.y, 0.0f }, bottom_left.c },
		top_right,
		{ { bottom_left.p.x, top_right.p.y, 0.0f }, top_right.c }
	}
{
}


template <typename T>
VkVertexInputBindingDescription get_bindings()
{
	VkVertexInputBindingDescription bindings = {};
	bindings.binding = 0;
	bindings.stride = sizeof( T );
	bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindings;
}


template <>
std::vector<VkVertexInputAttributeDescription> get_attributes<Dot>()
{
	std::vector<VkVertexInputAttributeDescription> attributes( 2 );

	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof( Dot, p );

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributes[1].offset = offsetof( Dot, c );

	return attributes;
}

template <>
std::vector<VkVertexInputAttributeDescription> get_attributes<Vertex>()
{
	std::vector<VkVertexInputAttributeDescription> attributes( 4 );

	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = offsetof( Vertex, p );

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof( Vertex, n );

	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributes[2].offset = offsetof( Vertex, c );

	attributes[3].binding = 0;
	attributes[3].location = 3;
	attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[3].offset = offsetof( Vertex, t );

	return attributes;
}


PhysicalDevice::PhysicalDevice( VkPhysicalDevice h )
: handle { h }
{
	vkGetPhysicalDeviceProperties( handle, &properties );

	uint32_t queue_family_count;
	vkGetPhysicalDeviceQueueFamilyProperties( handle, &queue_family_count, nullptr );
	queue_families.resize( queue_family_count );
	vkGetPhysicalDeviceQueueFamilyProperties( handle, &queue_family_count, queue_families.data() );

	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties( handle, nullptr, &extension_count, nullptr );
	extensions.resize( extension_count );
	vkEnumerateDeviceExtensionProperties( handle, nullptr, &extension_count, extensions.data() );
}


VkSurfaceCapabilitiesKHR PhysicalDevice::get_capabilities( const VkSurfaceKHR s )
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( handle, s, &capabilities );
	return capabilities;
}


std::vector<VkSurfaceFormatKHR> PhysicalDevice::get_formats( VkSurfaceKHR s )
{
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR( handle, s, &format_count, nullptr);
	std::vector<VkSurfaceFormatKHR> formats( format_count );
	vkGetPhysicalDeviceSurfaceFormatsKHR( handle, s, &format_count, formats.data() );
	return formats;
}


std::vector<VkPresentModeKHR> PhysicalDevice::get_present_modes( VkSurfaceKHR s )
{
	uint32_t mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR( handle, s, &mode_count, nullptr );
	std::vector<VkPresentModeKHR> modes( mode_count );
	vkGetPhysicalDeviceSurfacePresentModesKHR( handle, s, &mode_count, modes.data() );
	return modes;
}


uint32_t PhysicalDevice::get_memory_type( uint32_t type_filter, VkMemoryPropertyFlags flags )
{
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties( handle, &properties );

	for ( uint32_t i = 0; i < properties.memoryTypeCount; ++i )
	{
		if ( ( type_filter & ( 1 << i ) ) &&
		     ( ( properties.memoryTypes[i].propertyFlags & flags ) == flags ) )
		{
			return i;
		}
	}

	assert( false && "Cannot find memory type" );
	return 0;
}


VkFormatProperties PhysicalDevice::get_format_properties( const VkFormat format )
{
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties( handle, format, &props );
	return props;
}


Queue::Queue( const Device& d, const uint32_t family_index, const uint32_t index )
: device { d }
, family_index { family_index }
, index { index }
, flags { d.physical_device.queue_families[family_index].queueFlags }
{
	vkGetDeviceQueue( d.handle, family_index, index, &handle );
}


bool Queue::supports_present( VkSurfaceKHR s )
{
	VkBool32 support_present;
	vkGetPhysicalDeviceSurfaceSupportKHR( device.physical_device.handle, family_index, s, &support_present );
	return support_present == VK_TRUE;
}


void Queue::submit( CommandBuffer& command_buffer, const std::vector<VkSemaphore>& waits, const std::vector<VkSemaphore>& signals, Fence* fence )
{
	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = waits.size();
	info.pWaitSemaphores = waits.data();

	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	info.pWaitDstStageMask = wait_stages;

	info.commandBufferCount = 1;
	info.pCommandBuffers = &command_buffer.handle;

	info.signalSemaphoreCount = signals.size();
	info.pSignalSemaphores = signals.data();

	auto fence_handle = fence ? fence->handle : VK_NULL_HANDLE;

	const auto res = vkQueueSubmit( handle, 1, &info, fence_handle );
	assert( res == VK_SUCCESS && "Cannot submit to queue" );

	fence->can_wait = true;
}


VkResult Queue::present( const std::vector<VkSwapchainKHR>& swapchains, const std::vector<uint32_t>& image_indices, const std::vector<VkSemaphore>& waits )
{
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = waits.size();
	info.pWaitSemaphores = waits.data();

	info.swapchainCount = swapchains.size();
	info.pSwapchains = swapchains.data();
	info.pImageIndices = image_indices.data();

	return vkQueuePresentKHR( handle, &info );
}

Device::Device( PhysicalDevice& d, const VkSurfaceKHR s, const RequiredExtensions required_extensions )
: physical_device { d }
, surface { s }
{
	VkDeviceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	// Queue infos
	VkDeviceQueueCreateInfo queue_info = {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.queueFamilyIndex = 0;
	queue_info.queueCount = 1;
	float queue_priority = 1.0f;
	queue_info.pQueuePriorities = &queue_priority;

	info.queueCreateInfoCount = 1;
	info.pQueueCreateInfos = &queue_info;

	// Features
	VkPhysicalDeviceFeatures features = {};
	features.samplerAnisotropy = VK_TRUE;

	info.pEnabledFeatures = &features;

	// Extensions
	if ( required_extensions.count > 0 )
	{
		info.enabledExtensionCount = required_extensions.count;
		info.ppEnabledExtensionNames = required_extensions.names;
	}

	auto ret = vkCreateDevice( d.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create device" );

	// Get graphics queue
	queues.emplace_back( *this, 0, 0 );
	assert( queues[0].handle != VK_NULL_HANDLE && "Cannot get graphics queue" );
	assert( queues[0].flags & VK_QUEUE_GRAPHICS_BIT && "First queue is not for graphics" );
	assert( queues[0].supports_present( surface ) && "First queue does not support present" );
}


Device::~Device()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyDevice( handle, nullptr );
	}
}


Queue& Device::find_queue( VkQueueFlagBits flags )
{
	auto it = std::find_if( std::begin( queues ), std::end( queues ),
		[flags]( auto& queue ) { return queue.flags & flags; });
	assert( it != std::end( queues ) && "Cannot find graphics queue" );
	return *it;
}


void Device::wait_idle() const
{
	auto res = vkDeviceWaitIdle( handle );
	assert( res == VK_SUCCESS && "Cannot wait for device to be idle" );
}


Queue& Device::find_graphics_queue()
{
	return find_queue( VK_QUEUE_GRAPHICS_BIT );
}


Queue& Device::find_present_queue( VkSurfaceKHR surface )
{
	auto it = std::find_if( std::begin( queues ), std::end( queues ),
		[surface]( gfx::Queue& queue ) { return queue.supports_present( surface ); });
	assert( it != std::end( queues ) && "Cannot find graphics queue" );
	return *it;
}


Instance::Instance( RequiredExtensions req_ext, ValidationLayers layers )
{
	VkInstanceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.enabledExtensionCount = req_ext.count;
	info.ppEnabledExtensionNames = req_ext.names;
	info.enabledLayerCount = layers.count;
	info.ppEnabledLayerNames = layers.names;
	auto res = vkCreateInstance( &info, nullptr, &handle );
	assert( res == VK_SUCCESS && "Cannot create Vulkan instance" );

	uint32_t ph_dev_count;
	vkEnumeratePhysicalDevices( handle, &ph_dev_count, nullptr );

	std::vector<VkPhysicalDevice> ph_devs( ph_dev_count );
	vkEnumeratePhysicalDevices( handle, &ph_dev_count, ph_devs.data() );

	for ( auto& ph_dev : ph_devs )
	{
		physical_devices.emplace_back( ph_dev );
	}
}


Instance::~Instance()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyInstance( handle, nullptr );
	}
}


Semaphore::Semaphore( Device& d )
: device { d }
{
	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	auto ret = vkCreateSemaphore( device.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create sempahore" );
}


Semaphore::~Semaphore()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroySemaphore( device.handle, handle, nullptr );
	}
}


Semaphore::Semaphore( Semaphore&& other )
: device { other.device }
, handle { other.handle }
{
	other.handle = VK_NULL_HANDLE;
}


Semaphore& Semaphore::operator=( Semaphore&& other )
{
	assert( &device == &other.device && "Cannot move semaphore from different device" );
	std::swap(handle, other.handle);
	return *this;
}


Fence::Fence( Device& d )
: device { d }
{
	VkFenceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	auto res = vkCreateFence( device.handle, &info, nullptr, &handle );
	assert( res == VK_SUCCESS && "Cannot create fence" );
}


Fence::~Fence()
{
	if ( handle != VK_NULL_HANDLE )
	{
		wait();
		vkDestroyFence( device.handle, handle, nullptr );
	}
}


Fence::Fence( Fence&& other )
: device { other.device }
, handle { other.handle }
{
	other.handle = VK_NULL_HANDLE;
}


Fence& Fence::operator=( Fence&& other )
{
	assert( device.handle == other.device.handle
		&& "Cannot move assign fence of another device" );
	std::swap( handle, other.handle );
	return *this;
}


void Fence::wait() const
{
	if ( can_wait )
	{
		auto res = vkWaitForFences( device.handle, 1, &handle, VK_TRUE, std::numeric_limits<uint64_t>::max() );
		assert( res == VK_SUCCESS && "Cannot wait for fence" );
	}
}


void Fence::reset()
{
	auto res = vkResetFences( device.handle, 1, &handle );
	assert( res == VK_SUCCESS && "Cannot reset fence" );
	can_wait = false;
}


VkSurfaceFormatKHR choose_format( const std::vector<VkSurfaceFormatKHR>& formats )
{
	assert( formats.size() > 0 && "No surface formats available" );

	for ( auto& format : formats )
	{
		if ( format.format == VK_FORMAT_B8G8R8A8_UNORM &&
		     format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
		{
			return format;
		}
	}

	return formats[0];
}


VkExtent2D choose_extent( const VkSurfaceCapabilitiesKHR& capabilities )
{
	if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
	{
	//	return capabilities.currentExtent;
		return capabilities.minImageExtent;
	}

	return capabilities.minImageExtent;
}


VkPresentModeKHR choose_present_mode( const std::vector<VkPresentModeKHR>& present_modes)
{
	for ( auto mode : present_modes )
	{
		if ( mode == VK_PRESENT_MODE_MAILBOX_KHR )
		{
			return mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}


Swapchain::Swapchain( Device& d )
: device { d }
{
	create();
}


void Swapchain::create()
{
	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.surface = device.surface;

	auto capabilities = device.physical_device.get_capabilities( device.surface );
	info.minImageCount = 3;
	assert( capabilities.maxImageCount == 0 || capabilities.maxImageCount > 2 &&
		"Triple buffering is not supported");
	if ( capabilities.maxImageCount > 0 && info.minImageCount > capabilities.maxImageCount )
	{
		info.minImageCount = capabilities.maxImageCount;
	}

	auto formats = device.physical_device.get_formats( device.surface );
	auto surface_format = choose_format( formats );
	format = surface_format.format;
	info.imageFormat = surface_format.format;
	info.imageColorSpace = surface_format.colorSpace;

	extent = choose_extent( capabilities );
	info.imageExtent = extent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	info.preTransform = capabilities.currentTransform;

	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	auto present_modes = device.physical_device.get_present_modes( device.surface );
	info.presentMode = choose_present_mode( present_modes );
	info.clipped = VK_TRUE;

	info.oldSwapchain = handle;

	auto ret = vkCreateSwapchainKHR( device.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create swapchain" );

	uint32_t image_count;
	vkGetSwapchainImagesKHR( device.handle, handle, &image_count, nullptr );

	assert( images.size() == 0 && "Images should not be there" );
	images.resize( image_count );
	vkGetSwapchainImagesKHR( device.handle, handle, &image_count, images.data() );

	assert( views.size() == 0 && "Views should not be there" );
	views.resize( image_count );
	for ( size_t i = 0; i < image_count; ++i )
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = images[i];
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;
		info.components.r = info.components.g = info.components.b = info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.layerCount = 1;

		auto ret = vkCreateImageView( device.handle, &info, nullptr, views.data() + i );
		assert( ret == VK_SUCCESS && "Cannote create swapchain image view" );
	}
}

void Swapchain::destroy_views()
{
	for ( auto view : views )
	{
		vkDestroyImageView( device.handle, view, nullptr );
	}
	views.clear();
	images.clear();
}


Swapchain::~Swapchain()
{
	destroy_views();

	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroySwapchainKHR( device.handle, handle, nullptr );
	}
}


Frames::Frames( Swapchain& swapchain )
{
	for ( size_t i = 0; i < swapchain.images.size(); ++i )
	{
		color_images.emplace_back( swapchain.images[i] );
		color_views.emplace_back( swapchain.views[i] );

		auto depth = Image( swapchain.device, swapchain.extent, VK_FORMAT_D32_SFLOAT );
		depth.transition( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		auto view = ImageView( depth );
		depth_images.emplace_back( std::move( depth ) );
		depth_views.emplace_back( std::move( view ) );
	}
}


std::vector<Framebuffer> Frames::create_framebuffers( RenderPass& render_pass )
{
	std::vector<Framebuffer> framebuffers;
	for ( size_t i = 0; i < color_views.size(); ++i )
	{
		auto color_view = color_views[i];
		auto depth_view = depth_views[i].handle;
		std::vector<VkImageView> views = { color_view, depth_view };
		VkExtent2D extent = { depth_images[i].extent.width, depth_images[i].extent.height };
		auto framebuffer = Framebuffer( views, extent, render_pass );
		framebuffers.emplace_back( std::move( framebuffer ) );
	}
	return framebuffers;
}


void Swapchain::recreate()
{
	device.wait_idle();
	destroy_views();
	create();
}


ShaderModule::ShaderModule( Device& d, const std::filesystem::path& path )
: device { d }
{
	VkShaderModuleCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = std::filesystem::file_size(path);

	std::vector<char> code( info.codeSize );
	std::ifstream( path ).read( code.data(), info.codeSize );
	info.pCode = reinterpret_cast<uint32_t*>( code.data() );

	const auto res = vkCreateShaderModule(device.handle, &info, nullptr, &handle);
	assert( res == VK_SUCCESS && "Cannot create shader" );
}


RenderPass::RenderPass( Swapchain& s )
: device { s.device }
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = s.format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = VK_FORMAT_D32_SFLOAT;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_ref = {};
	color_ref.attachment = 0;
	color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_ref = {};
	depth_ref.attachment = 1;
	depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_ref;
	subpass.pDepthStencilAttachment = &depth_ref;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {
		color_attachment, depth_attachment
	};

	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount = attachments.size();
	info.pAttachments = attachments.data();
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;

	auto ret = vkCreateRenderPass( device.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create render pass" );
}


RenderPass::~RenderPass()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyRenderPass( device.handle, handle, nullptr );
	}
}


RenderPass& RenderPass::operator=( RenderPass&& other )
{
	assert( device.handle == other.device.handle &&
		"Cannot move assign render pass of another device" );
	std::swap( handle, other.handle );
	return *this;
}


Framebuffer::Framebuffer( const std::vector<VkImageView>& views, VkExtent2D& ext, RenderPass& render_pass )
: device { render_pass.device }
, extent { ext }
{
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = render_pass.handle;
	info.attachmentCount = views.size();
	info.pAttachments = views.data();
	info.width = extent.width;
	info.height = extent.height;
	info.layers = 1;

	auto ret = vkCreateFramebuffer( device.handle, &info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannot create framebuffer" );
}


Framebuffer::~Framebuffer()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyFramebuffer( device.handle, handle, nullptr );
	}
}


Framebuffer::Framebuffer( Framebuffer&& other )
: device { other.device }
, handle { other.handle }
, extent { other.extent }
{
	other.handle = VK_NULL_HANDLE;
}


ShaderModule::~ShaderModule()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyShaderModule( device.handle, handle, nullptr );
	}
}


ShaderModule::ShaderModule( ShaderModule&& other )
: device { other.device }
, handle { other.handle }
{
	other.handle = VK_NULL_HANDLE;
}


ShaderModule& ShaderModule::operator=( ShaderModule&& other )
{
	assert( &device == &other.device );
	std::swap( handle, other.handle );
	return *this;
}


ValidationLayers get_validation_layers()
{
	static std::vector<const char*> layer_names = {
		"VK_LAYER_KHRONOS_validation"
	};

	ValidationLayers layers = {};
	layers.names = layer_names.data();
	layers.count = layer_names.size();

	return layers;
}


VkViewport create_viewport( const Glfw::Window& window )
{
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = window.extent.width;
	viewport.height = window.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	return viewport;
}


VkRect2D create_scissor( const Glfw::Window& window )
{
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = window.extent;
	return scissor;
}


std::vector<VkDescriptorSetLayoutBinding> get_line_bindings()
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = 0;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	return { binding };
}


std::vector<VkDescriptorSetLayoutBinding> get_mesh_bindings()
{
	VkDescriptorSetLayoutBinding ubo_binding = {};
	ubo_binding.binding = 0;
	ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_binding.descriptorCount = 1;
	ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding material_binding = {};
	material_binding.binding = 1;
	material_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	material_binding.descriptorCount = 1;
	material_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding sampler_binding = {};
	sampler_binding.binding = 2;
	sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_binding.descriptorCount = 1;
	sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return { ubo_binding, material_binding, sampler_binding };
}


std::vector<VkDescriptorSetLayoutBinding> get_mesh_no_image_bindings()
{
	VkDescriptorSetLayoutBinding ubo_binding = {};
	ubo_binding.binding = 0;
	ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_binding.descriptorCount = 1;
	ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding material_binding = {};
	material_binding.binding = 1;
	material_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	material_binding.descriptorCount = 1;
	material_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return { ubo_binding, material_binding };
}


mth::Mat4 perspective( const float a, const float y, const float f, const float n )
{
	assert( f > n && "Far should be greater than near" );

	mth::Mat4 proj = {};

	// Calculate projection matrix
	float cotfov = 1.0f / std::tan( 0.5f * y );
	proj.matrix[0] = cotfov / a;
	proj.matrix[5] = -cotfov;
	proj.matrix[10] = -( n + f ) / ( f - n );
	proj.matrix[14] = -2.0f * n * f / ( f - n );
	proj.matrix[11] = -1.0f;

	return proj;
}


mth::Mat4 look_at( const mth::Vec3& eye, const mth::Vec3& center, mth::Vec3 up )
{
	mth::Vec3 forward = eye - center;
	forward.normalize();

	mth::Vec3 right = mth::Vec3::cross( up, forward );
	right.normalize();

	up = mth::Vec3::cross( forward, right );
	up.normalize();

	mth::Mat4 matrix = {};

	matrix[0][0] = right.x;
	matrix[1][0] = right.y;
	matrix[2][0] = right.z;
	matrix[3][0] = -mth::Vec3::dot( right, eye );
	matrix[0][1] = up.x;
	matrix[1][1] = up.y;
	matrix[2][1] = up.z;
	matrix[3][1] = -mth::Vec3::dot( up, eye );
	matrix[0][2] = forward.x;
	matrix[1][2] = forward.y;
	matrix[2][2] = forward.z;
	matrix[3][2] = -mth::Vec3::dot( forward, eye );
	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
	matrix[3][3] = 1.0f;

	return matrix;
}

mth::Mat4 ortho( float left, float right, float bottom, float top, float near, float far )
{
	mth::Vec3 mid;
	mid.x = ( left + right ) / ( right - left );
	mid.y = ( bottom + top ) / ( bottom - top );
	mid.z = near / ( near - far );

	mth::Vec3 scale;
	scale.x = 2.0f / ( right - left );
	scale.y = 2.0f / ( bottom - top );
	scale.z = 1.0f / ( near - far );

	mth::Mat4 mat = mth::Mat4::identity;

	mat.matrix[12] = -mid.x;
	mat.matrix[13] = -mid.y;
	mat.matrix[14] = mid.z;

	mat.matrix[0] = scale.x;
	mat.matrix[5] = -scale.y;
	mat.matrix[10] = scale.z;

	return mat;
}


Graphics::Graphics()
: instance { glfw.required_extensions, get_validation_layers() }
, surface { instance, window }
, device { instance.physical_devices.at( 0 ), surface.handle, device_required_extensions }
, swapchain { device }
, frames { swapchain }
, render_pass { swapchain }
, line_vert { device, "test/shader/line.vert.spv" }
, line_frag { device, "test/shader/line.frag.spv" }
, line_layout { device, get_line_bindings() }
, mesh_vert { device, "test/shader/mesh.vert.spv" }
, mesh_frag { device, "test/shader/mesh.frag.spv" }
, mesh_no_image_vert { device, "test/shader/mesh-no-image.vert.spv" }
, mesh_no_image_frag { device, "test/shader/mesh-no-image.frag.spv" }
, mesh_layout { device, get_mesh_bindings() }
, mesh_no_image_layout { device, get_mesh_no_image_bindings() }
, viewport { create_viewport( window ) }
, scissor { create_scissor( window ) }
, line_pipeline {
	get_bindings<Dot>(),
	get_attributes<Dot>(),
	line_layout,
	line_vert,
	line_frag,
	render_pass,
	viewport,
	scissor,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST }
, dot_pipeline {
	get_bindings<Dot>(),
	get_attributes<Dot>(),
	line_layout,
	line_vert,
	line_frag,
	render_pass,
	viewport,
	scissor,
	VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
, mesh_pipeline {
	get_bindings<Vertex>(),
	get_attributes<Vertex>(),
	mesh_layout,
	mesh_vert,
	mesh_frag,
	render_pass,
	viewport,
	scissor }
, mesh_no_image_pipeline {
	get_bindings<Vertex>(),
	get_attributes<Vertex>(),
	mesh_no_image_layout,
	mesh_no_image_vert,
	mesh_no_image_frag,
	render_pass,
	viewport,
	scissor }
, renderer { *this }
, command_pool { device }
, command_buffers { command_pool.allocate_command_buffers( swapchain.images.size() ) }
, framebuffers { frames.create_framebuffers( render_pass ) }
, graphics_queue { device.find_graphics_queue() }
, present_queue { device.find_present_queue( surface.handle ) }
, images { device }
, models { *this }
, view { look_at(
	mth::Vec3( 0.0f, 0.0f, 0.0f ),
	mth::Vec3( 0.0f, 0.0f, 0.0f ),
	mth::Vec3( 0.0f, 1.0f, 0.0f ) ) }
, proj { perspective( swapchain.extent.width / float(swapchain.extent.height), mth::radians( 60.0f ), 10000.0f, 0.125f ) }
{
	for ( size_t i = 0; i < swapchain.images.size(); ++i )
	{
		images_available.emplace_back( device );
		images_drawn.emplace_back( device );
		frames_in_flight.emplace_back( device );
	}
}


bool Graphics::render_begin()
{
	std::rotate(std::begin(images_available), ++std::begin(images_available), std::end(images_available));
	current_image_available = &images_available.back();

	uint32_t image_index;
	auto res = vkAcquireNextImageKHR( device.handle,
		swapchain.handle,
		std::numeric_limits<uint64_t>::max(),
		current_image_available->handle,
		VK_NULL_HANDLE,
		&image_index );
	if ( res == VK_ERROR_OUT_OF_DATE_KHR ||
		viewport.width != window.extent.width ||
		viewport.height != window.extent.height )
	{
		// Recreate current semaphore
		images_available.back() = Semaphore( device );

		swapchain.recreate();
		render_pass = RenderPass( swapchain );

		// Update viewport and scissor
		viewport.width = window.extent.width;
		viewport.height = window.extent.height;
		scissor.extent = window.extent;

		dot_pipeline = GraphicsPipeline(
			get_bindings<Dot>(),
			get_attributes<Dot>(),
			line_layout,
			line_vert,
			line_frag,
			render_pass,
			viewport,
			scissor, VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
		line_pipeline = GraphicsPipeline(
			get_bindings<Dot>(),
			get_attributes<Dot>(),
			line_layout,
			line_vert,
			line_frag,
			render_pass,
			viewport,
			scissor, VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
		mesh_pipeline = GraphicsPipeline(
			get_bindings<Vertex>(),
			get_attributes<Vertex>(),
			mesh_layout,
			mesh_vert,
			mesh_frag,
			render_pass,
			viewport,
			scissor );
		mesh_no_image_pipeline = GraphicsPipeline(
			get_bindings<Vertex>(),
			get_attributes<Vertex>(),
			mesh_no_image_layout,
			mesh_no_image_vert,
			mesh_no_image_frag,
			render_pass,
			viewport,
			scissor );

		frames = Frames( swapchain );
		framebuffers = frames.create_framebuffers( render_pass );

		proj = perspective(
			viewport.width / viewport.height,
			mth::radians( 60.0f ),
			10000.0f,
			0.125f );

		for ( auto& fence : frames_in_flight )
		{
			fence.can_wait = false;
		}

		return false; // then skip frame
	}
	else
	{
		assert( ( res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR ) && "Cannot present" );
	}

	current_frame_index = image_index;
	current_frame_in_flight = &frames_in_flight[current_frame_index];
	current_frame_in_flight->wait();
	current_frame_in_flight->reset();

	current_command_buffer = &command_buffers[image_index];
	current_framebuffer = &framebuffers[image_index];

	current_command_buffer->begin();
	current_command_buffer->begin_render_pass( render_pass, *current_framebuffer );

	return true;
}


void Graphics::render_end()
{
	current_command_buffer->end_render_pass();
	current_command_buffer->end();

	auto& image_drawn = images_drawn[current_frame_index];

	graphics_queue.submit( *current_command_buffer, { current_image_available->handle }, { image_drawn.handle }, current_frame_in_flight );

	present_queue.present( { swapchain.handle }, { current_frame_index }, { image_drawn.handle } );
}


void Graphics::draw( Line& line )
{
	auto& resources = renderer.line_resources.find( &line )->second;

	line.ubo.view = view;
	line.ubo.proj = proj;

	auto data = reinterpret_cast<const uint8_t*>( &line.ubo );
	auto& uniform_buffer = resources.uniform_buffers[current_frame_index];
	uniform_buffer.upload( data, sizeof( UniformBufferObject ) );

	current_command_buffer->bind( line_pipeline );
	current_command_buffer->bind_vertex_buffers( resources.vertex_buffer );
	current_command_buffer->bind_index_buffer( resources.index_buffer );

	auto& descriptor_set = resources.descriptor_sets[current_frame_index];
	current_command_buffer->bind_descriptor_sets( line_layout, descriptor_set );
	current_command_buffer->draw_indexed( resources.index_buffer.count() );
}



void Graphics::draw( Triangle& tri )
{
	auto& resources = renderer.triangle_resources.find( &tri )->second;

	tri.ubo.view = view;
	tri.ubo.proj = proj;

	auto data = reinterpret_cast<const uint8_t*>( &tri.ubo );
	auto& uniform_buffer = resources.uniform_buffers[current_frame_index];
	uniform_buffer.upload( data, sizeof( UniformBufferObject ) );

	current_command_buffer->bind( line_pipeline );
	current_command_buffer->bind_vertex_buffers( resources.vertex_buffer );
	current_command_buffer->bind_index_buffer( resources.index_buffer );

	auto& descriptor_set = resources.descriptor_sets[current_frame_index];
	current_command_buffer->bind_descriptor_sets( line_layout, descriptor_set );
	current_command_buffer->draw_indexed( resources.index_buffer.count() );
}


void Graphics::draw( Rect& rect )
{
	auto& resources = renderer.rect_resources.find( &rect )->second;

	rect.ubo.view = view;
	rect.ubo.proj = proj;

	auto data = reinterpret_cast<const uint8_t*>( &rect.ubo );
	auto& uniform_buffer = resources.uniform_buffers[current_frame_index];
	uniform_buffer.upload( data, sizeof( UniformBufferObject ) );

	current_command_buffer->bind( line_pipeline );
	current_command_buffer->bind_vertex_buffers( resources.vertex_buffer );
	current_command_buffer->bind_index_buffer( resources.index_buffer );

	auto& descriptor_set = resources.descriptor_sets[current_frame_index];
	current_command_buffer->bind_descriptor_sets( line_layout, descriptor_set );
	current_command_buffer->draw_indexed( resources.index_buffer.count() );
}


void Graphics::draw( const gtf::Node& node, Mesh& mesh, const mth::Mat4& transform )
{
	for( auto& primitive : mesh.primitives )
	{
		auto& layout = primitive.material->texture == VK_NULL_HANDLE ? mesh_no_image_layout : mesh_layout;

		auto pair = renderer.mesh_resources.find( hash( node, primitive ) );
		assert( pair != std::end( renderer.mesh_resources ) && "Cannot find resources for a mesh" );
		auto& resources = pair->second;

		UniformBufferObject ubo;
		ubo.model = transform;
		ubo.view  = view;
		ubo.proj  = proj;

		auto data = reinterpret_cast<const uint8_t*>( &ubo );
		auto& uniform_buffer = resources.uniform_buffers[current_frame_index];
		uniform_buffer.upload( data, sizeof( UniformBufferObject ) );

		auto material_data = reinterpret_cast<const uint8_t*>( &primitive.material->ubo );
		auto& material_ubo = resources.material_ubos[current_frame_index];
		material_ubo.upload( material_data, sizeof( Material::Ubo ) );

		auto& pipeline = primitive.material->texture == VK_NULL_HANDLE ? mesh_no_image_pipeline : mesh_pipeline;
		current_command_buffer->bind( pipeline );
		current_command_buffer->bind_vertex_buffer( resources.vertex_buffer );
		current_command_buffer->bind_index_buffer( resources.index_buffer );

		auto& descriptor_set = resources.descriptor_sets[current_frame_index];
		current_command_buffer->bind_descriptor_sets( layout, descriptor_set );
		current_command_buffer->draw_indexed( primitive.indices.size() );
	}
}


void Graphics::draw( const gtf::Node& node, const mth::Mat4& transform )
{
	// Current transform
	auto temp_transform = node.matrix;
	temp_transform.scale( node.scale );
	temp_transform.rotate( node.rotation );
	temp_transform.translate( node.translation );
	temp_transform = transform * temp_transform;

	// Render its children
	for ( auto child : node.children )
	{
		draw( *child, temp_transform );
	}

	// Render the node
	if ( node.mesh_index >= 0 )
	{
		auto& mesh = models.meshes[node.mesh_index];
		draw( node, mesh, temp_transform );
	}
}


void Graphics::draw( const gtf::Scene& scene )
{
	std::for_each( std::begin( scene.nodes ), std::end( scene.nodes ), [this]( auto n ) {
		draw( models.nodes[n] );
	});
}



} // namespace spot::gfx
