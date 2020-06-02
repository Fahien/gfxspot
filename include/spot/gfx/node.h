#pragma once

#include <vector>
#include <spot/math/math.h>
#include <spot/math/mat4.h>

#include "spot/handle.h"


namespace spot::gfx
{
class Model;
class GltfCamera;
class Mesh;
class Light;
class Script;
class Shape;
class Bounds;
class Rect;


/// Node in the node hierarchy
class Node : public Handled<Node>
{
  public:
	/// @return A newly created Node
	Node() = default;

	/// @param mesh The mesh of this node
	/// @return A newly created Node
	Node( const Handle<Mesh>& mesh );

	/// @param name Name of the node
	/// @return A newly created Node
	Node( const std::string& name );

	void invalidate() override;

	const math::Vec3& get_translation() const { return translation; }
	const math::Quat& get_rotation() const { return rotation; }
	const math::Vec3& get_scaling() const { return scaling; }

	void set_translation( const math::Vec3& t ) { dirty = true; translation = t; };
	void set_translation( const math::Vec2& t ) { dirty = true; translation = t; };
	void set_translation_x( float v ) { dirty = true; translation.x = v; };
	void set_translation_y( float v ) { dirty = true; translation.y = v; };
	void set_translation_z( float v ) { dirty = true; translation.z = v; };

	void set_rotation( const math::Quat& r ) { dirty = true; rotation = r; };
	void set_scaling( const math::Vec3& s ) { dirty = true; scaling = s; };

	Node& translate( const math::Vec3& translation );
	Node& translate( float x, float y ) { dirty = true; translation.x += x; translation.y += y; return *this; }
	Node& rotate( const math::Quat& rotation );
	Node& scale( const math::Vec3& scale );
	Node& scale( float s ) { dirty = true; scaling *= s; return *this; }

	/// @brief Updates transform matrices of this node and its children
	void update_transforms( const math::Mat4& transform = math::Mat4::Identity );

	/// @brief Updates transform matrices of this node and its children
	void recalculate( math::Mat4 transform = math::Mat4::Identity );

	/// @return The current transform
	const math::Mat4& get_transform() const;

	/// @return A matrix representing the absolute transform of this node
	math::Mat4 get_absolute_matrix() const;

	/// @param name Name of the node
	/// @return A newly created Node as a child of this
	Handle<Node> create_child( const std::string& name );

	/// @brief Add a new child to this node
	void add_child( const Handle<Node>& child );

	/// @brief Set this node free!
	void remove_from_parent();

	/// Handle of the mesh of the node
	Handle<Mesh> mesh = {};

	/// Handle of light contained by this node
	Handle<Light> light = {};

	/// User-defined name of this object
	std::string name = "Unknown";

	/// Parent of this node
	Handle<Node> parent;

	/// This node's children
	std::vector<Handle<Node>> children;

	/// This node's rectangle bounds
	Handle<Bounds> bounds;
	Handle<Rect> rect;

  private:
	/// Whether node's transform needs recalculation
	bool dirty = true;

	/// Floating-point 4x4 transformation matrix stored in column-major order
	math::Mat4 matrix = math::Mat4::Identity;

	/// Unit quaternion
	math::Quat rotation = math::Quat::Identity;

	/// Non-uniform scale
	math::Vec3 scaling = { 1.0f, 1.0f, 1.0f };

	/// Translation
	math::Vec3 translation = { 0.0f, 0.0f, 0.0f };

	/// Camera referenced by this node
	GltfCamera* camera = nullptr;

	/// This node's scripts indices
	std::vector<size_t> scripts_indices;

	/// This node's scripts
	std::vector<Script*> scripts;

	friend class Model;
};


}  // namespace spot::gfx
