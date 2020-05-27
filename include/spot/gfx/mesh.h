#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <spot/math/math.h>
#include <spot/math/shape.h>

#include "spot/handle.h"
#include "spot/gltf/color.h"


namespace spot::gfx
{
struct Accessor;
struct Material;
class Mesh;

struct alignas(16) Vertex
{
	Vertex( math::Vec3 pp = {}, Color cc = { 1.0f, 1.0f, 1.0f, 1.0f }, math::Vec2 tc = {} ) : p { pp }, c { cc }, t { tc } {}

	math::Vec3  p = {};
	math::Vec3  n = { 1.0f, 1.0f, 1.0f };
	Color c = {};
	math::Vec2  t = {};
};


/// @todo Change this, as indices can be of different sizes
using Index = uint16_t;


/// @brief Geometry to be rendered with the given material
struct Primitive
{
	/// Enumerated value identifying the vertex attribute
	enum class Semantic
	{
		NONE,
		POSITION,
		NORMAL,
		TANGENT,
		TEXCOORD_0,
		TEXCOORD_1,
		COLOR_0,
		JOINTS_0,
		WEIGHTS_0
	};

	/// Type of primitives to render
	enum class Mode
	{
		POINTS,
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};

	Primitive() = default;

	Primitive(
		std::vector<Vertex> vertices,
		std::vector<Index> indices,
		const Handle<Material>& material
	);

	/// Dictionary object, where each key corresponds to mesh attribute semantic and
	/// each value is a handle to the accessor containing attribute's data (required)
	std::unordered_map<Semantic, Handle<Accessor>> attributes;

	/// Handle to the accessor that contains the indices
	Handle<Accessor> indices_handle = {};

	/// Material to apply to this primitive when rendering
	Handle<Material> material = {};

	/// Type of primitives to render
	Mode mode = Mode::TRIANGLES;

	/// Depth of line to use for line topology
	float line_width = 1.0f;

	/// targets TODO An array of Morph Targets, each Morph Target is a dictionary mapping attributes (only POSITION,
	/// NORMAL, and TANGENT supported) to their deviations in the Morph Target extensions TODO Dictionary object with
	/// extension-specific objects Application-specific data
	void* extras;

	std::vector<Vertex> vertices;
	std::vector<Index> indices;
};


/// @brief Set of primitives to be rendered
struct Mesh : public Handled<Mesh>
{
	/// @return A colored line mesh
	static Mesh create_line( const math::Vec3& a, const math::Vec3& b, const Color& c = Color::White, float line_width = 1.0f );

	/// @return A triangle mesh; material is optional
	static Mesh create_triangle( const math::Vec3& a, const math::Vec3& b, const math::Vec3& c, const Handle<Material>& material = {} );

	/// @return A rectangle mesh with a material
	static Mesh create_rect( const math::Vec3& a, const math::Vec3& b, const Handle<Material>& material );
	static Mesh create_rect( const math::Rect& r, const Handle<Material>& material );

	/// @return A colored rectangle mesh without material
	static Mesh create_rect( const math::Vec3& a, const math::Vec3& b, const Color& c = Color::White );
	static Mesh create_rect( const math::Rect& r, const Color& c = Color::White );

	/// @return A quad mesh, which is a unit square with a material
	static Mesh create_quad(
		const Material& material,
		const math::Vec3& a = { -0.5f, -0.5f, 0.0f },
		const math::Vec3& b = { 0.5f, 0.5f, 0.0f }
	);

	/// @return A unit cube mesh
	static Mesh create_cube( const Handle<Material>& m );

	Mesh( std::vector<Primitive>&& ps = {} ) : primitives { std::move( ps ) } {}

	/// Array of primitives, each defining geometry to be rendered with a material (required)
	std::vector<Primitive> primitives;

	/// Array of weights to be applied to the Morph Targets
	std::vector<float> weights;

	/// User-defined name of this object
	std::string name = "Unknown";

	/// extensions TODO Dictionary object with extension-specific objects
	/// extras TODO Application-specific data
	void* extras;
};


}  // namespace spot::gfx
