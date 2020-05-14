#pragma once

#include "spot/gfx/vulkan.h"

namespace spot::gfx
{
class CommandBuffer;
class Fence;

class Queue
{
  public:
	Queue( const Device& d, uint32_t family_index, uint32_t index );

	bool supports_present( VkSurfaceKHR s );

	void submit( CommandBuffer& command_buffer, const std::vector<VkSemaphore>& waits,
	             const std::vector<VkSemaphore>& signals = {}, Fence* fence = nullptr );

	VkResult present( const std::vector<VkSwapchainKHR>& swapchains, const std::vector<uint32_t>& image_index,
	                  const std::vector<VkSemaphore>& waits = {} );

	const Device& device;
	uint32_t      family_index = 0;
	uint32_t      index = 0;
	VkQueue       handle = VK_NULL_HANDLE;
	VkQueueFlags  flags = 0;
};


class Device
{
  public:
	Device( PhysicalDevice& p, VkSurfaceKHR s, RequiredExtensions required_extensions = {} );
	~Device();

	bool operator==( const Device& o ) const { return handle == o.handle; }

	void   wait_idle() const;
	Queue& find_queue( VkQueueFlagBits flags );
	Queue& find_graphics_queue();
	Queue& find_present_queue( VkSurfaceKHR surface );

	PhysicalDevice&    physical_device;
	VkSurfaceKHR       surface;
	VkDevice           handle = VK_NULL_HANDLE;
	std::vector<Queue> queues;
};


}  // namespace spot::gfx
