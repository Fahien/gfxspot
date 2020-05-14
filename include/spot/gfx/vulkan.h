#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

#include "spot/gfx/vulkan_utils.h"

namespace spot::gfx
{
class PhysicalDevice
{
  public:
	PhysicalDevice( VkPhysicalDevice h );

	VkSurfaceCapabilitiesKHR get_capabilities( VkSurfaceKHR s );

	/// @return Supported surface formats
	std::vector<VkSurfaceFormatKHR> get_formats( VkSurfaceKHR s );

	std::vector<VkPresentModeKHR> get_present_modes( VkSurfaceKHR s );
	uint32_t                      get_memory_type( uint32_t type_filter, VkMemoryPropertyFlags f );
	VkFormatProperties            get_format_properties( VkFormat f ) const;

	/// @return Whether a format is supported by the GPU
	bool supports( VkFormat f ) const;

	VkPhysicalDevice                     handle = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties           properties;
	VkPhysicalDeviceFeatures             features;
	std::vector<VkQueueFamilyProperties> queue_families;
	std::vector<VkExtensionProperties>   extensions;
};

class Instance
{
  public:
	Instance( RequiredExtensions req_ext = {}, ValidationLayers layers = {} );
	~Instance();

	VkInstance                  handle = VK_NULL_HANDLE;
	std::vector<PhysicalDevice> physical_devices;
};


}  // namespace spot::gfx