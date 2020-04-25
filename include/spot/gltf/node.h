#pragma once

#include <vector>
#include <spot/math/math.h>

#include "spot/gltf/handle.h"


namespace spot::gfx
{
class Gltf;
class GltfCamera;
class Mesh;
class Light;
class Script;
class Shape;


/// Node in the node hierarchy
class Node : public Handled<Node>
{
  public:
	bool contains( const math::Vec2& point ) const;

	/// @return The parent of this node, otherwise nullptr
	Handle<Node> get_parent() const;

	/// @return A list of children of this node
	const std::vector<Handle<Node>>& get_children() const { return children; }

	/// @return The current transform
	math::Mat4 get_matrix() const;

	/// @return A matrix representing the absolute transform of this node
	math::Mat4 get_absolute_matrix() const;

	/// @param name Name of the node
	/// @return A newly created Node as a child of this
	Handle<Node> create_child( const std::string& name );

	/// @brief Add a new child to this node
	void add_child( Handle<Node>& child );

	void remove_from_parent();

	/// Unit quaternion
	math::Quat rotation = math::Quat::identity;

	/// Non-uniform scale
	math::Vec3 scale = math::Vec3{ 1.0f, 1.0f, 1.0f };

	/// Translation
	math::Vec3 translation = math::Vec3{ 0.0f, 0.0f, 0.0f };

	/// Handle of the mesh of the node
	Handle<Mesh> mesh = {};

	/// User-defined name of this object
	std::string name = "Unknown";

  private:
	/// Gltf owning the node
	Gltf* model = nullptr;

	/// Parent of this node
	Handle<Node> parent = {};

	/// This node's children
	std::vector<Handle<Node>> children;

	/// Floating-point 4x4 transformation matrix stored in column-major order
	math::Mat4 matrix = math::Mat4::identity;

	/// Camera referenced by this node
	GltfCamera* camera = nullptr;

	/// If not negative, index of light contained by this node
	int32_t light_index = -1;

	/// This node's bounds indices
	int32_t bounds = -1;

	/// This node's scripts indices
	std::vector<size_t> scripts_indices;

	/// This node's scripts
	std::vector<Script*> scripts;

	friend class Gltf;
};


/// Root nodes of a scene
struct Scene
{
	/// Gltf owning the scene
	Gltf* model = nullptr;
	
	/// Indices of each root node
	std::vector<Handle<Node>> nodes;
	
	/// User-defined name of this object
	std::string name = "default";

	/// @param name Name of the node
	/// @return A newly created Node as root of a scene
	Handle<Node> create_node( const std::string& name = {} );
};


}  // namespace spot::gfx
