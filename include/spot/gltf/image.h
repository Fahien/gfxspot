#pragma once

#include <string>
#include "spot/handle.h"

namespace spot::gfx
{


/// @brief Image data used to create a texture
struct GltfImage : public Handled<GltfImage>
{
	/// Uri of the image
	std::string uri = "";

	/// MIME type
	std::string mime_type = "";

	/// Buffer view index
	uint32_t buffer_view = 0;

	/// Name
	std::string name = "Unknown";
};


}  // namespace spot::gfx
