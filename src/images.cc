#include "spot/gfx/images.h"

#include <cassert>

#include "spot/gfx/png.h"
#include "spot/gfx/graphics.h"

namespace spot::gfx
{

VkFormat get_format( Png& png )
{
	switch ( png.color_type )
	{
	case PNG_COLOR_TYPE_RGB:
	{
		assert( png.bit_depth == 8 && png.channels == 3 );
		return VK_FORMAT_R8G8B8_UNORM;
	}
	case PNG_COLOR_TYPE_RGB_ALPHA:
	{
		assert( png.bit_depth == 8 && png.channels == 4 );
		return VK_FORMAT_R8G8B8A8_UNORM;
	}
	default:
		assert( false && "Vulkan format not supported" );
	}

	return VK_FORMAT_UNDEFINED;
}


Image::Image( Device& d, Png& png )
: Image { d, { png.width, png.height }, get_format( png ) }
{}


Image::Image( Device& d, const VkExtent2D ext, const VkFormat fmt )
: device { d }
, extent { ext.width, ext.height, 1 }
, format { fmt }
, command_pool { d }
{
	// Check format
	assert( d.physical_device.supports( format ) && "VkFormat not supported" );
	auto props = d.physical_device.get_format_properties( fmt );

	// Select proper tiling
	VkImageTiling tiling = {};
	if ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT )
	{
		tiling = VK_IMAGE_TILING_OPTIMAL;
	}
	else if ( props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT )
	{
		tiling = VK_IMAGE_TILING_LINEAR;
	}
	else
	{
		assert( false && "Current device cannot sample from this format" );
	}

	// Image
	VkImageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.extent = extent;
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.tiling = tiling;
	info.format = format;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	if ( format == VK_FORMAT_D32_SFLOAT )
	{
		info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.samples = VK_SAMPLE_COUNT_1_BIT;

	auto res = vkCreateImage( device.handle, &info, nullptr, &vkhandle );
	assert( res == VK_SUCCESS && "Cannot create image" );

	// Memory
	{
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements( device.handle, vkhandle, &requirements);

		VkMemoryAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info.allocationSize = requirements.size;
		auto memory_type = device.physical_device.get_memory_type(
			requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		info.memoryTypeIndex = memory_type;

		auto res = vkAllocateMemory( device.handle, &info, nullptr, &memory );
		assert( res == VK_SUCCESS && "Cannot allocate memory for image" );

		res = vkBindImageMemory( device.handle, vkhandle, memory, 0 );
		assert( res == VK_SUCCESS && "Cannot bind image memory" );
	}
}


Image::~Image()
{
	if ( vkhandle != VK_NULL_HANDLE )
	{
		vkFreeMemory( device.handle, memory, nullptr );
		vkDestroyImage( device.handle, vkhandle, nullptr );
	}
}


Image::Image( Image&& other )
: device { other.device }
, extent { other.extent }
, format { other.format }
, vkhandle { other.vkhandle }
, memory { other.memory }
, layout { other.layout }
, command_pool { std::move( other.command_pool ) }
{
	other.vkhandle = VK_NULL_HANDLE;
	other.memory = VK_NULL_HANDLE;
}


Image& Image::operator=( Image&& other )
{
	assert( device.handle == other.device.handle && "Cannot move images from different device" );
	std::swap( extent, other.extent );
	std::swap( format, other.format );
	std::swap( handle, other.handle );
	std::swap( memory, other.memory );
	std::swap( layout, other.layout );
	std::swap( command_pool, other.command_pool );

	return *this;
}


void Image::transition( const VkImageLayout new_layout )
{
	auto cmds = command_pool.allocate_command_buffers();
	auto& cmd = cmds[0];

	cmd.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
	cmd.transition( *this, new_layout );
	cmd.end();

	auto& queue = device.find_graphics_queue();
	auto fence = Fence( device );
	fence.reset();
	queue.submit( cmd, {}, {}, &fence );
}


void Image::upload( Buffer& buffer )
{
	auto cmds = command_pool.allocate_command_buffers();
	auto& cmd = cmds[0];

	cmd.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

	cmd.transition( *this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
	cmd.copy( buffer, *this );
	cmd.transition( *this, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

	cmd.end();

	auto& queue = device.find_graphics_queue();
	auto fence = Fence( device );
	fence.reset();
	queue.submit( cmd, {}, {}, &fence );
}


VkImageAspectFlags get_aspect( const Image& image )
{
	if ( image.format == VK_FORMAT_D32_SFLOAT )
	{
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else
	{
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}


ImageView::ImageView( const Handle<Image>& img )
: ImageView( *img )
{
	image = img;
}


ImageView::ImageView( const Image& image )
: device { image.device }
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.image = image.vkhandle;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = image.format;
	info.subresourceRange.aspectMask = get_aspect( image );
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	auto res = vkCreateImageView( device.handle, &info, nullptr, &vkhandle );
	assert( res == VK_SUCCESS && "Cannot create image view" );
}


ImageView::~ImageView()
{
	if ( vkhandle != VK_NULL_HANDLE )
	{
		vkDestroyImageView( device.handle, vkhandle, nullptr );
	}
}


ImageView::ImageView( ImageView&& other )
: device { other.device }
, image { std::move( other.image ) }
, vkhandle { other.vkhandle }
{
	other.vkhandle = VK_NULL_HANDLE;
}


ImageView& ImageView::operator=( ImageView&& other )
{
	assert( device.handle == other.device.handle && "Cannot move images from different device" );
	std::swap( image, other.image );
	std::swap( handle, other.handle );

	return *this;
}


Sampler::Sampler( Device& d )
: device { d }
{
	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.magFilter = VK_FILTER_NEAREST;
	info.minFilter = VK_FILTER_NEAREST;

	info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	info.anisotropyEnable = VK_FALSE;
	info.maxAnisotropy = 16;

	info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	info.unnormalizedCoordinates = VK_FALSE;

	info.compareEnable = VK_FALSE;
	info.compareOp = VK_COMPARE_OP_ALWAYS;

	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	info.mipLodBias = 0.0f;
	info.minLod = 0.0f;
	info.maxLod = 0.0f;

	auto res = vkCreateSampler( device.handle, &info, nullptr, &handle );
	assert( res == VK_SUCCESS && "Cannot create sampler" );
}


Sampler::~Sampler()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroySampler( device.handle, handle, nullptr );
	}
}


Sampler::Sampler( Sampler&& other )
: device { other.device }
, handle { other.handle }
{
	other.handle = VK_NULL_HANDLE;
}


Sampler& Sampler::operator=( Sampler&& other )
{
	assert( device.handle == other.device.handle && "Cannot move sampler from different device" );
	std::swap( handle, other.handle );
	return *this;
}


Images::Images( Device& d )
: device { d }
{}


Handle<ImageView> Images::load( const char* name, std::vector<uint8_t>& mem )
{
	Handle<ImageView> ret = {};

	auto it = std::find(
		std::begin( image_paths ),
		std::end( image_paths ),
		 name );
	if ( it == std::end( image_paths ) )
	{
		auto png = Png( mem );
		auto png_size = png.get_size();
		auto staging_buffer = Buffer( device, png_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT );
		auto mem = reinterpret_cast<png_byte*>( staging_buffer.map( png_size ) );
		png.load( mem );
		staging_buffer.unmap();

		image_paths.emplace_back( name );
		auto image = images.push( Image( device, png ) );
		image->upload( staging_buffer );
		ret = views.push( ImageView( image ) );
	}
	else
	{
		auto index = it - std::begin( image_paths );
		ret = views.find( index );
	}

	return ret;
}


Handle<ImageView> Images::load( const char* path )
{
	Handle<ImageView> ret = {};

	auto it = std::find(
		std::begin( image_paths ),
		std::end( image_paths ),
		path );
	if ( it == std::end( image_paths ) )
	{
		auto png = Png( path );
		auto png_size = png.get_size();
		auto staging_buffer = Buffer( device, png_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT );
		auto mem = reinterpret_cast<png_byte*>( staging_buffer.map( png_size ) );
		png.load( mem );
		staging_buffer.unmap();

		image_paths.emplace_back( path );
		auto image = images.push( Image( device, png ) );
		image->upload( staging_buffer );
		ret = views.push( ImageView( image ) );
	}
	else
	{
		auto index = it - std::begin( image_paths );
		ret = views.find( index );
	}

	return ret;
}

Images::Images( Images&& o )
: device { o.device }
, images { std::move( o.images ) }
{}

Images& Images::operator=( Images&& o )
{
	assert( device == o.device && "Images are not from the same device" );
	std::swap( images, o.images );
	return *this;
}


} // namespace spot::gfx
