#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "spot/gltf/handle.h"
#include "spot/gltf/color.h"

namespace spot::gfx
{
struct GltfTexture;

/// Material appearance of a primitive
struct Material : public Handled<Material>
{
	Material() = default;
	Material( const Color& c ) : pbr { c } {}
	Material( VkImageView t ) : texture { t } {}

	/// Metallic-Roughness Material
	struct alignas(16) PbrMetallicRoughness
	{
		/// Base color of the material
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

		/// Metalness of the material
		float metallic = 1.0f;

		/// Roughness of the material
		float roughness = 1.0f;

		float ambient_occlusion = 0.5f;
	};

	/// User-defined name of this object
	std::string name = "Unknown";

	/// Metallic-Roughness Material
	PbrMetallicRoughness pbr;

	/// Base color texture
	Handle<GltfTexture> texture_handle = {};

	VkImageView texture = VK_NULL_HANDLE;
};


}  // namespace spot::gfx
