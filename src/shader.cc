#include "spot/gfx/shader.h"

#include <cassert>
#include <vector>
#include <fstream>

#include "spot/gfx/device.h"

namespace spot::gfx
{
ShaderModule::ShaderModule( Device& d, const std::filesystem::path& path )
    : device { d }
{
	VkShaderModuleCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = std::filesystem::file_size( path );

	std::vector<char> code( info.codeSize );
	std::ifstream( path, std::ios::binary ).read( code.data(), info.codeSize );
	info.pCode = reinterpret_cast<uint32_t*>( code.data() );

	const auto res = vkCreateShaderModule( device.handle, &info, nullptr, &handle );
	assert( res == VK_SUCCESS && "Cannot create shader" );
}

}  // namespace spot::gfx
