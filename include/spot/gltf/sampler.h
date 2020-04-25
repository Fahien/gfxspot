#ifndef GST_SAMPLER_H_
#define GST_SAMPLER_H_

#include <string>

namespace spot::gfx
{
/// Texture sampler properties for filtering and wrapping modes
struct GltfSampler
{
	/// Magnification/Minification filter
	enum class Filter
	{
		NONE                   = 0,
		NEAREST                = 9728,
		LINEAR                 = 9729,
		NEAREST_MIPMAP_NEAREST = 9984,
		LINEAR_MIPMAP_NEAREST  = 9985,
		NEAREST_MIPMAP_LINEAR  = 9986,
		LINEAR_MIPMAP_LINEAR   = 9987
	};

	/// Wrapping mode
	enum class Wrapping
	{
		CLAMP_TO_EDGE   = 33071,
		MIRRORED_REPEAT = 33648,
		REPEAT          = 10497
	};

	/// Magnification filter
	Filter magFilter = Filter::NONE;

	/// Minification filter
	Filter minFilter = Filter::NONE;

	/// S wrapping mode
	Wrapping wrapS = Wrapping::REPEAT;

	/// T wrapping mode
	Wrapping wrapT = Wrapping::REPEAT;

	/// User-defined name of this object
	std::string name = "Unknown";
};


}  // namespace spot::gfx


#endif  // GST_SAMPLER_H_
