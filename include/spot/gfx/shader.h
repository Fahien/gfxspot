#pragma once

#include <filesystem>

#include <vulkan/vulkan_core.h>


namespace spot::gfx
{
class Device;

class ShaderModule
{
  public:
	ShaderModule( Device& d, const std::filesystem::path& path );
	~ShaderModule();

	ShaderModule( ShaderModule&& );
	ShaderModule& operator=( ShaderModule&& );

	Device&        device;
	VkShaderModule handle = VK_NULL_HANDLE;
};


}  // namespace spot::gfx