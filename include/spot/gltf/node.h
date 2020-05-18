#pragma once

#include <vector>
#include <spot/math/math.h>

#include "spot/handle.h"


namespace spot::gfx
{
class Gltf;
class GltfCamera;
class Mesh;
class Light;
class Script;
class Shape;
class Bounds;


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

	/// @brief Updates transform matrices of this node and its children
	void update_transforms( const math::Mat4& transform = math::Mat4::Identity );

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

	/// @return The bounds of the node, nullptr otherwise
	Bounds* get_bounds() const;

	/// @param b Set new bounds for this node
	void set_bounds( const Handle<Bounds>& b );

	/// Unit quaternion
	math::Quat rotation = math::Quat::Identity;

	/// Non-uniform scale
	math::Vec3 scale = math::Vec3{ 1.0f, 1.0f, 1.0f };

	/// Translation
	math::Vec3 translation = math::Vec3{ 0.0f, 0.0f, 0.0f };

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

	/// This node's bounds handle
	Handle<Bounds> bounds = {};

  private:
	/// Floating-point 4x4 transformation matrix stored in column-major order
	math::Mat4 matrix = math::Mat4::Identity;

	/// Camera referenced by this node
	GltfCamera* camera = nullptr;

	/// This node's scripts indices
	std::vector<size_t> scripts_indices;

	/// This node's scripts
	std::vector<Script*> scripts;

	friend class Gltf;
};


}  // namespace spot::gfx
