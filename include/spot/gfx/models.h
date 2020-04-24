#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include <spot/math/math.h>
#include <spot/math/shape.h>
#include <spot/gltf/gltf.h>

#include "spot/gfx/images.h"


namespace spot::gfx
{

class Graphics;

struct alignas(16) UniformBufferObject
{
	math::Mat4 model = math::Mat4::identity;
	math::Mat4 view  = math::Mat4::identity;
	math::Mat4 proj  = math::Mat4::identity;
};


/// @brief Models stores everything needed by a scene loaded into the engine
/// Images, materials, meshes, etcetera
class Models
{
  public:
	Models( Graphics& g );

	/// @brief Loads a gltf file
	/// @return A reference to the first scene
	Scene& load( const std::string& path );

	/// @return A node index for the text
	Handle<Node> create_text( const std::string& text );

	Graphics& graphics;

	Images images;

	Gltf gltf;

  private:
;
};


} // namespace spot::gfx
