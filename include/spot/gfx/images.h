#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>
#include <spot/handle.h>

#include "spot/gfx/commands.h"


namespace spot::gfx
{

class Device;
class Png;
class Buffer;

class VulkanImage : public Handled<VulkanImage>
{
  public:
	/// @param p PNG file to load into the Vulkan image
	VulkanImage( Device& d, Png& p );
	VulkanImage( Device& d, VkExtent2D e, VkFormat f );
	~VulkanImage();

	VulkanImage( VulkanImage&& o );
	VulkanImage& operator=( VulkanImage&& o );

	void transition( const VkImageLayout l );
	void upload( Buffer& b );

	Device& device;
	VkExtent3D extent = {};
	VkFormat format = VK_FORMAT_UNDEFINED;

	VkImage vkhandle = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
	CommandPool command_pool;
};


class ImageView : public Handled<ImageView>
{
  public:
	ImageView( const VulkanImage& i );
	ImageView( const Handle<VulkanImage>& i );
	~ImageView();

	ImageView( ImageView&& o );
	ImageView& operator=( ImageView&& o );

	Device& device;
	Handle<VulkanImage> image = {};

	VkImageView vkhandle = VK_NULL_HANDLE;
};


class Sampler
{
  public:
	Sampler( Device& d );
	~Sampler();

	Sampler( Sampler&& o );
	Sampler& operator=( Sampler&& o );

	Device& device;
	VkSampler handle = VK_NULL_HANDLE;
};


/// @brief VulkanImage repository
class Images
{
  public:
	Images( Device& d );

	Images( Images&& o );
	Images& operator=( Images&& o );

	/// @brief Loads an image from file
	/// @return A handle of an image view into that image
	Handle<ImageView> load( const char* path );

	/// @brief Loads an image from a vector of bytes
	/// @return A handle of an image view into that image
	Handle<ImageView> load( const char* name, std::vector<uint8_t>& mem );
	
	/// @brief Loads an image from raw memory
	/// @return A handle of an image view into that image
	Handle<ImageView> load( const char* name, const uint8_t* mem, size_t size, const VkExtent2D& extent );

	/// Map of paths and Vulkan images and image views
	std::vector<std::string> image_paths;
	Uvec<VulkanImage> images;
	Uvec<ImageView> views;

	Device& device;
};


} // namespace spot::gfx
