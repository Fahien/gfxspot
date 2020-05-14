#include "spot/gfx/graphics.h"

#include <array>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cmath>
#include <spot/log.h>

#include <spot/gltf/mesh.h>
#include <spot/gltf/material.h>

#include "spot/gfx/hash.h"

namespace gtf = spot::gfx;


namespace spot::gfx
{


PhysicalDevice::PhysicalDevice( VkPhysicalDevice h )
: handle { h }
{
	vkGetPhysicalDeviceProperties( handle, &properties );
	vkGetPhysicalDeviceFeatures( handle, &features );

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


VkFormatProperties PhysicalDevice::get_format_properties( const VkFormat format ) const
{
	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties( handle, format, &props );
	return props;
}


bool PhysicalDevice::supports( const VkFormat format ) const
{
	VkFormatProperties zero = {};
	auto props = get_format_properties( format );
	auto res = std::memcmp( &props, &zero, sizeof( props ) );
	return res != 0;
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

	if ( fence )
	{
		fence->can_wait = true;
	}
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

	if ( physical_device.features.wideLines == VK_TRUE )
	{
		features.wideLines = VK_TRUE;
	}

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
		wait_idle();
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
		auto depth_view = depth_views[i].vkhandle;
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
	info.codeSize = std::filesystem::file_size( path );

	std::vector<char> code( info.codeSize );
	std::ifstream( path, std::ios::binary ).read( code.data(), info.codeSize );
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


VkRect2D create_scissor( const Window& window )
{
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = window.frame;
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


std::vector<VkDescriptorSetLayoutBinding> get_mesh_no_image_bindings()
{
	std::vector<VkDescriptorSetLayoutBinding> ret;

	auto& mvp = ret.emplace_back();
	mvp.binding = ret.size() - 1;
	mvp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	mvp.descriptorCount = 1;
	mvp.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

//	auto& ambient = ret.emplace_back();
//	ambient.binding = ret.size() - 1;
//	ambient.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	ambient.descriptorCount = 1;
//	ambient.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//	auto& light = ret.emplace_back();
//	light.binding = ret.size() - 1;
//	light.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	light.descriptorCount = 1;
//	light.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	auto& material = ret.emplace_back();
	material.binding = ret.size() - 1;
	material.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	material.descriptorCount = 1;
	material.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return ret;
}


std::vector<VkDescriptorSetLayoutBinding> get_mesh_bindings()
{
	auto ret = get_mesh_no_image_bindings();

	auto& sampler = ret.emplace_back();
	sampler.binding = ret.size() - 1;
	sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler.descriptorCount = 1;
	sampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return ret;
}


Graphics::Graphics()
: instance { glfw.required_extensions, get_validation_layers() }
, surface { instance, window }
, device { instance.physical_devices.at( 0 ), surface.handle, device_required_extensions }
, swapchain { device }
, frames { swapchain }
, render_pass { swapchain }
, line_vert { device, "shader/line.vert.spv" }
, line_frag { device, "shader/line.frag.spv" }
, line_layout { device, get_line_bindings() }
, mesh_vert { device, "shader/mesh.vert.spv" }
, mesh_frag { device, "shader/mesh.frag.spv" }
, mesh_no_image_vert { device, "shader/mesh-no-image.vert.spv" }
, mesh_no_image_frag { device, "shader/mesh-no-image.frag.spv" }
, mesh_layout { device, get_mesh_bindings() }
, mesh_no_image_layout { device, get_mesh_no_image_bindings() }
, gui { device, window.frame }
, viewport { window, camera }
, scissor { create_scissor( window ) }
, renderer { *this }
, command_pool { device }
, command_buffers { command_pool.allocate_command_buffers( swapchain.images.size() ) }
, framebuffers { frames.create_framebuffers( render_pass ) }
, graphics_queue { device.find_graphics_queue() }
, present_queue { device.find_present_queue( surface.handle ) }
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
		viewport.get_viewport().width != window.frame.width ||
		viewport.get_viewport().height != window.frame.height )
	{
		// Recreate current semaphore
		images_available.back() = Semaphore( device );

		swapchain.recreate();
		render_pass = RenderPass( swapchain );

		// Update viewport and scissor
		viewport.update();
		scissor.extent = window.frame;

		renderer.recreate_pipelines();

		frames = Frames( swapchain );
		framebuffers = frames.create_framebuffers( render_pass );


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


void Graphics::draw_gui()
{
	auto [vertex_data, index_data] = gui_draw();
	if ( vertex_data.empty() )
	{
		return;
	}

	/// @todo Draw gui, TRY with push constants for scale and translate
	/// @todo Fix this 3
	current_command_buffer->bind( renderer.pipelines[3] );

	// UI scale and translate via push constants
	Constants constants;
	auto& display_size = ImGui::GetIO().DisplaySize;
	constants.transform.translate( math::Vec3( -1.0f, -1.0f ) );
	constants.transform.scale( math::Vec3( 2.0f / display_size.x, 2.0f / display_size.y ) );

	current_command_buffer->push_constants( gui.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( Constants ), &constants );

	auto& gui_descriptor_sets = renderer.gui_resources.descriptor_sets[current_frame_index];
	current_command_buffer->bind_descriptor_sets( gui.layout, gui_descriptor_sets );

	auto& vertex_buffer = renderer.gui_resources.vertex_buffers[current_frame_index];
	/// @todo Improve this approach with DynamicBuffers
	vertex_buffer.set_count( vertex_data.size() );
	vertex_buffer.upload( reinterpret_cast<uint8_t*>( vertex_data.data() ) );

	auto& index_buffer = renderer.gui_resources.index_buffers[current_frame_index];
	index_buffer.set_count( index_data.size() );
	index_buffer.upload( reinterpret_cast<uint8_t*>( index_data.data() ) );

	current_command_buffer->bind_vertex_buffer( vertex_buffer );
	current_command_buffer->bind_index_buffer( index_buffer );

	gui.draw( *current_command_buffer );
}

void Graphics::render_end()
{
	draw_gui();

	current_command_buffer->end_render_pass();
	current_command_buffer->end();

	auto& image_drawn = images_drawn[current_frame_index];

	graphics_queue.submit( *current_command_buffer, { current_image_available->handle }, { image_drawn.handle }, current_frame_in_flight );

	present_queue.present( { swapchain.handle }, { current_frame_index }, { image_drawn.handle } );
}


void Graphics::draw( const Handle<Node>& node, const Handle<Mesh>& mesh, const math::Mat4& transform )
{
	for ( auto& prim : mesh->primitives )
	{
		draw( node, prim, transform );
	}
}


void Graphics::draw( const Handle<Node>& node, const Primitive& primitive, const math::Mat4& transform )
{
	auto node_pair = renderer.node_resources.find( node );
	if ( node_pair == std::end( renderer.node_resources ) )
	{
		renderer.add( node );
		return;
	}

	auto hash_prim = std::hash<Primitive>()( primitive );
	auto& resources = renderer.primitive_resources.at( hash_prim );

	// Upload MVP UBO
	MvpUbo ubo;
	ubo.model = transform;
	ubo.view  = camera.get_view();
	ubo.proj  = camera.get_proj();

	auto data = reinterpret_cast<const uint8_t*>( &ubo );

	auto& node_resources = node_pair->second;
	auto& uniform_buffer = node_resources.ubos[current_frame_index];
	uniform_buffer.upload( data, sizeof( MvpUbo ) );

	size_t hash_desc = std::hash_combine( node, primitive.material );
	auto desc_it = renderer.descriptor_resources.find( hash_desc );
	if ( desc_it == std::end( renderer.descriptor_resources ) )
	{
		desc_it = renderer.add_descriptors( node, primitive.material );
	}
	auto& descriptor_resources = desc_it->second;
	auto& pipeline = renderer.pipelines[descriptor_resources.pipeline];
	current_command_buffer->bind( pipeline );

	if ( primitive.material )
	{
		// Upload Material UBO
		auto material_data = reinterpret_cast<const uint8_t*>( &primitive.material->pbr );
		auto& material_resources = renderer.material_resources.at( primitive.material );
		auto& material_ubo = material_resources.ubos[current_frame_index];
		material_ubo.upload( material_data, sizeof( Material::PbrMetallicRoughness ) );

		// Upload Ambient UBO
		if ( light_node )
		{
			auto ambient_data = reinterpret_cast<const uint8_t*>( &ambient.ubo );
			auto& ambient_ubo = renderer.ambient_resources.ubos[current_frame_index];
			ambient_ubo.upload( ambient_data, sizeof( Ambient::Ubo ) );

			// Upload Light UBO
			LightUbo light_ubo = {};
			light_ubo.position = light_node->translation;
			light_ubo.color = light_node->light->color;
			auto light_data = reinterpret_cast<const uint8_t*>( &light_ubo );
			auto& light_res = renderer.light_resources.begin()->second;
			auto& light_buffer = light_res.ubos[current_frame_index];
			light_buffer.upload( light_data, sizeof( LightUbo ) );
		}
	}
	else // Draw mesh with lines
	{
		// Check wideline support
		if ( device.physical_device.features.wideLines == VK_TRUE )
		{
			current_command_buffer->set_line_width( primitive.line_width );
		}
	}

	current_command_buffer->bind_vertex_buffer( resources.vertex_buffer );
	current_command_buffer->bind_index_buffer( resources.index_buffer );

	auto& descriptor_set = descriptor_resources.descriptor_sets[current_frame_index];
	current_command_buffer->bind_descriptor_sets( pipeline.layout, descriptor_set );
	current_command_buffer->draw_indexed( primitive.indices.size() );
}


void Graphics::draw( const Handle<Node>& node, const math::Mat4& transform )
{
	// Current transform
	auto temp_transform = node->get_matrix();
	temp_transform = transform * temp_transform;

	// Render its children
	for ( auto& child : node->children )
	{
		draw( child, temp_transform );
	}

	// Render the node
	if ( node->mesh )
	{
		for ( auto& primitive : node->mesh->primitives )
		{
			draw( node, primitive, temp_transform );
		}
	}
}


void Graphics::draw( const Handle<Gltf>& model, const math::Mat4& transform )
{
	if ( !model->scene )
	{
		return;
	}

	for ( auto& node : model->scene->nodes )
	{
		draw( node, transform );
	}
}


} // namespace spot::gfx
