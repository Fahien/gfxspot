#pragma once

#include <string>

#include <spot/handle.h>

namespace spot::gfx
{

struct GltfSampler;
struct Image;


/// Texture and its sampler
struct GltfTexture : public Handled<GltfTexture>
{
	/// Sampler used by this texture
	Handle<GltfSampler> sampler;

	/// Image used by this texture
	Handle<Image> source;

	/// User-defined name of this object
	std::string name = "Unknown";
};


}  // namespace spot::gfx
