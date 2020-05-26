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


/// @brief Models stores everything needed by a scene loaded into the engine
/// Images, materials, meshes, etcetera
class Models
{
  public:
	Models( Graphics& g ) : gfx { g } {}

	/// @brief Loads a gltf file
	/// @return A handle to the gltf model
	Handle<Model> load( const std::string& path );

	/// @return A node index for the text
	Handle<Node> create_text( const std::string& text );

	Graphics& gfx;
};


} // namespace spot::gfx
