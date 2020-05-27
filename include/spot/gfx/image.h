#pragma once

#include <string>

#include <spot/handle.h>

namespace spot::gfx
{


/// @brief Image data used to create a texture
struct Image : public Handled<Image>
{
	Image( const std::string& path = "" );

	/// Uri of the image
	std::string uri = "";

	/// MIME type
	std::string mime_type = "";

	/// Buffer view index
	uint32_t buffer_view = 0;

	/// Name
	std::string name = "Unknown";

	uint32_t width = 0;
	uint32_t height = 0;
};


}  // namespace spot::gfx
