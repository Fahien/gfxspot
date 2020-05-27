#pragma once

#include <string>

#include <spot/handle.h>

namespace spot::gfx
{

struct GltfSampler;
struct Image;


/// @brief Texture and its sampler
struct Texture : public Handled<Texture>
{
	Texture( const Handle<Image>& image = {} ) : source { image } {}

	/// Sampler used by this texture
	Handle<GltfSampler> sampler;

	/// Image used by this texture
	Handle<Image> source;

	/// User-defined name of this object
	std::string name = "Unknown";
};


}  // namespace spot::gfx
