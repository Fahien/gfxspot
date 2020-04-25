#pragma once

#include <string>

#include "spot/gltf/handle.h"

namespace spot::gfx
{

struct GltfSampler;
struct GltfImage;
class Gltf;


/// Texture and its sampler
struct GltfTexture
{
	Handle<GltfTexture> handle = {};

	/// Sampler used by this texture
	Handle<GltfSampler> sampler = {};

	/// Image used by this texture
	Handle<GltfImage> source = {};

	/// User-defined name of this object
	std::string name = "Unknown";
};


}  // namespace spot::gfx
