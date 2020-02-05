#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include <spot/gltf/gltf.h>
#include <spot/math/math.h>

#include "graphics/images.h"


namespace mth = spot::math;

namespace gfx
{

class Graphics;
class Buffer;


struct alignas(16) Color
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


struct alignas(16) Vec2
{
	Vec2( float xx = 0.0f, float yy = 0.0f ) : x { xx }, y { yy } {}

	float x = 0.0f;
	float y = 0.0f;
};

struct alignas(16) Vec3
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
	mth::Mat4 model = mth::Mat4::identity;
	mth::Mat4 view  = mth::Mat4::identity;
	mth::Mat4 proj  = mth::Mat4::identity;
};


using Index = uint16_t;

struct Primitive
{
	std::vector<Vertex> vertices = {};

	gfx::Buffer* vertex_buffer = nullptr;
	VkDeviceSize vertex_buffer_offset = 0;
	uint32_t stride = 0;

	std::vector<Index> indices = {};
	uint32_t indices_count = 0;

	gfx::Buffer* index_buffer = nullptr;
	VkDeviceSize index_buffer_offset = 0;

	Material* material = nullptr;

	/// @todo move this somewhere else
	UniformBufferObject ubo = {};
};


struct Mesh
{
	std::vector<Primitive> primitives;
};



class Models
{
  public:
	Models( Graphics& g );

	spot::gltf::Gltf& load( const std::string& path );

	Graphics& graphics;

	Images images;

	std::vector<gfx::Material> materials;

	std::vector<gfx::Mesh> meshes;

	std::vector<spot::gltf::Gltf> models;
};


} // namespace gfx
