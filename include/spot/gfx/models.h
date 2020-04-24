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

	Material( const Color& c = Color::white );
	Material( VkImageView texture );

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


struct alignas(16) Vertex
{
	Vertex( math::Vec3 pp = {}, Color cc = { 1.0f, 1.0f, 1.0f, 1.0f }, math::Vec2 tc = {} ) : p { pp }, c { cc }, t { tc } {}

	math::Vec3  p = {};
	math::Vec3  n = { 1.0f, 1.0f, 1.0f };
	Color c = {};
	math::Vec2  t = {};
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
class Primitive
{
  public:
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
	void set_material( int32_t m ) { material = m; }

private:
	/// An index to the material to use to draw the primitive
	int32_t material = -1;
};


/// @brief A mesh is just a collection of primitives
struct Mesh
{
	/// @return A colored line mesh
	static Mesh create_line( const math::Vec3& a, const math::Vec3& b, const Color& c = Color::white, float line_width = 1.0f );

	/// @return A triangle mesh; material is optional
	static Mesh create_triangle( const math::Vec3& a, const math::Vec3& b, const math::Vec3& c, int32_t material = -1 );

	/// @return A rectangle mesh with a material
	static Mesh create_rect( const math::Vec3& a, const math::Vec3& b, int32_t material );
	static Mesh create_rect( const math::Rect& r, int32_t material );

	/// @return A colored rectangle mesh without material
	static Mesh create_rect( const math::Vec3& a, const math::Vec3& b, const Color& c = Color::white );
	static Mesh create_rect( const math::Rect& r, const Color& c = Color::white );

	/// @return A quad mesh, which is a unit square with a material
	static Mesh create_quad(
		int32_t material,
		const math::Vec3& a = { -0.5f, -0.5f, 0.0f },
		const math::Vec3& b = { 0.5f, 0.5f, 0.0f }
	);
	static Mesh create_quad(
		const Material& material,
		const math::Vec3& a = { -0.5f, -0.5f, 0.0f },
		const math::Vec3& b = { 0.5f, 0.5f, 0.0f }
	);

	Mesh( std::vector<Primitive>&& ps = {} ) : primitives { std::move( ps ) } {}

	int32_t index = -1;

	std::vector<Primitive> primitives;
};


/// @brief Models stores everything needed by a scene loaded into the engine
/// Images, materials, meshes, etcetera
class Models
{
  public:
	Models( Graphics& g );

	/// @brief Loads a gltf file
	/// @return A reference to the first scene
	gltf::Scene& load( const std::string& path );

	/// @brief Creates a node with a new mesh
	/// @return The new created node
	gltf::Handle<gltf::Node> create_node( Mesh&& m );
	gltf::Handle<gltf::Node> create_node( Mesh&& m, gltf::Handle<gltf::Node> parent );

	/// @return The list of materials
	const std::vector<Material>& get_materials() const noexcept { return materials; }

	/// @return A new material with a proper index
	Material& create_material( Material&& m = {} );

	/// @return A new material with a solid color
	Material& create_material( const Color& c );

	/// @brief Creates a material with a texture
	/// @return A new material with a proper index
	Material& create_material( VkImageView texture );

	/// @return The material at index i, null otherwhise
	Material* get_material( int32_t i );

	/// @param material Material of this mesh
	/// @return A mesh with a proper index
	Mesh& create_mesh( Mesh&& mesh = {} );

	/// @return A node index for the text
	gltf::Handle<gltf::Node> create_text( const std::string& text );

	Graphics& graphics;

	Images images;

	std::vector<Mesh> meshes;

	gltf::Gltf gltf;

  private:

	/// Materials can be referred by multiple primitives
	std::vector<Material> materials;
};


} // namespace spot::gfx
