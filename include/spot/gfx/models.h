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
	static const Color black;
	static const Color white;
	static const Color red;
	static const Color green;
	static const Color blue;
	static const Color yellow;

	Color( float rr = 0.0f, float gg = 0.0f, float bb = 0.0f, float aa = 1.0f )
	: r { rr }, g { gg }, b { bb }, a { aa } {}

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};


struct Material
{
	static Material& get_black();
	static Material& get_white();
	static Material& get_yellow();
	static Material& get_red();

	struct alignas(16) Ubo
	{
		Color color;
		float metallic;
		float roughness;
		float ambient_occlusion;
	} ubo;

	VkImageView texture = VK_NULL_HANDLE;

	int32_t index = -1;
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
/// as it stores vertices, indices, and its material
struct Primitive
{
	Primitive() = default;

	Primitive(
		const std::vector<Vertex>& vv,
		const std::vector<Index>& ii,
		int32_t material = -1 );

	/// Vertices and indices do not change
	std::vector<Vertex> vertices = {};
	std::vector<Index> indices = {};

	/// Depth of line to use for line topology
	float line_width = 1.0f;

	int32_t get_material() const noexcept { return material; }

private:
	/// An index to the material to use to draw the primitive
	int32_t material = -1;
};


/// @brief A mesh is just a collection of primitives
struct Mesh
{
	static Mesh create_line( const Vec3& a, const Vec3& b, const Color& c = Color::white, float line_width = 1.0f );
	static Mesh create_triangle( const Vec3& a, const Vec3& b, const Vec3& c, int32_t material = -1 );
	static Mesh create_rect( const Vec3& a, const Vec3& b, int32_t material = -1 );

	static Mesh create_quad(
		const Vec3& a = { -1.0, -1.0, 0.0f },
		const Vec3& b = { 1.0f, 1.0f, 0.0f },
		int32_t material = -1
	);

	std::vector<Primitive> primitives;
};


/// @brief Models stores everything needed by a scene loaded into the engine
/// Images, materials, meshes, etcetera
class Models
{
  public:
	Models( Graphics& g );

	gltf::Scene& load( const std::string& path );

	/// @brief Creates a node and assign it an index
	/// @return The created node
	gltf::Node& create_node();

	/// @brief Creates a node with a new mesh
	/// @return The index of the node
	gltf::Node& create_node( Mesh&& m );

	/// @return The node at index i, null otherwhise
	gltf::Node* get_node( int32_t i );

	/// @return The list of materials
	const std::vector<Material>& get_materials() const noexcept { return materials; }

	/// @return A new material with a proper index
	Material& create_material( Material&& m = {} );

	/// @return The material at index i, null otherwhise
	gfx::Material* get_material( int32_t i );

	/// @return The list of nodes;
	const std::vector<gltf::Node>& get_nodes() const { return nodes; };

	Graphics& graphics;

	Images images;

	gltf::Scene scene;

	std::vector<gfx::Mesh> meshes;

  private:
	std::vector<gltf::Node> nodes;

	/// Materials can be referred by multiple primitives
	std::vector<gfx::Material> materials;
};


} // namespace spot::gfx
