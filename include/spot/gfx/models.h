#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include <spot/gltf/gltf.h>
#include <spot/math/math.h>

#include "spot/gfx/images.h"


namespace spot::gfx
{

class Graphics;
class Buffer;


struct Color
{
	Color( float rr = 0.0f, float gg = 0.0f, float bb = 0.0f, float aa = 1.0f )
	: r { rr }, g { gg }, b { bb }, a { aa } {}

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};


struct Material
{
	struct alignas(16) Ubo
	{
		Color color;
		float metallic;
		float roughness;
		float ambient_occlusion;
	} ubo;

	VkImageView texture = VK_NULL_HANDLE;
};


struct Vec2
{
	Vec2( float xx = 0.0f, float yy = 0.0f ) : x { xx }, y { yy } {}

	float x = 0.0f;
	float y = 0.0f;
};

struct Vec3
{
	Vec3( float xx = 0.0f, float yy = 0.0f, float zz = 0.0f )
	: x { xx }, y { yy }, z { zz } {}

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};


struct alignas(16) Vertex
{
	Vertex( Vec3 pp = {}, Color cc = { 1.0f, 1.0f, 1.0f, 1.0f }, Vec2 tc = {} ) : p { pp }, c { cc }, t { tc } {}

	Vec3  p = {};
	Vec3  n = { 1.0f, 1.0f, 1.0f };
	Color c = {};
	Vec2  t = {};
};


struct alignas(16) UniformBufferObject
{
	math::Mat4 model = math::Mat4::identity;
	math::Mat4 view  = math::Mat4::identity;
	math::Mat4 proj  = math::Mat4::identity;
};


/// @todo Change this, as indices can be of different sizes
using Index = uint16_t;


/// @brief A primitives has a central role in rendering
/// as it stores vertices, indices, and the material
struct Primitive
{
	/// Vertices and indices do not change
	std::vector<Vertex> vertices = {};
	std::vector<Index> indices = {};

	/// A material use to draw the primitive
	/// It can change at runtime
	Material* material = nullptr;

	/// @todo move this somewhere else
	UniformBufferObject ubo = {};
};


/// @brief A mesh is just a collection of primitives
struct Mesh
{
	std::vector<Primitive> primitives;
};


/// @brief Models stores everything needed by a scene loaded into the engine
/// Images, materials, meshes, etcetera
class Models
{
  public:
	Models( Graphics& g );

	gltf::Scene& load( const std::string& path );

	Graphics& graphics;

	Images images;

	std::vector<gltf::Node> nodes;
	gltf::Scene scene;

	/// Materials can be referred by multiple primitives
	std::vector<gfx::Material> materials;

	std::vector<gfx::Mesh> meshes;
};


} // namespace spot::gfx
