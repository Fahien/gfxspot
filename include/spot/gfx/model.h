#pragma once

#include <spot/math/math.h>
#include <spot/math/shape.h>
#include <nlohmann/json.hpp>

#include "spot/gltf/buffer.h"
#include "spot/gltf/camera.h"
#include "spot/gltf/image.h"
#include "spot/gltf/light.h"
#include "spot/gltf/material.h"
#include "spot/gltf/mesh.h"
#include "spot/gltf/sampler.h"
#include "spot/gltf/script.h"
#include "spot/gltf/texture.h"
#include "spot/gltf/bounds.h"
#include "spot/gltf/animation.h"
#include "spot/handle.h"

namespace spot::gfx
{

class Images;
class Model;

/// @brief Root nodes of a scene
struct Scene
{
	/// Model owning the scene
	Model* model = nullptr;
	
	/// Indices of each root node
	std::vector<Handle<Node>> nodes;
	
	/// User-defined name of this object
	std::string name = "default";

	/// @param name Name of the node
	/// @return A newly created Node as root of a scene
	Handle<Node> create_node( const std::string& name = {} );
};


/// @brief GL Transmission Format
class Model : public Handled<Model>
{
  public:
	/// Metadata about the glTF asset
	struct Asset
	{
		/// glTF version that this asset targets
		std::string version;
		/// Tool that generated this glTF model. Useful for debugging
		std::string generator;
		/// Copyright message suitable for display to credit the content creator
		std::string copyright;
	};

	friend class Node;
	friend class Scene;

	Model() = default;

	/// Loads a GLtf model from path
	/// @param path Gltf file path
	/// @return A Gltf model
	Model( const std::string& path );

	/// Constructs a Gltf object
	/// @param j Json object describing the model
	/// @param path Gltf file path
	Model( const nlohmann::json& j, const std::string& path = "." );

	/// Move contructs a Gltf object
	/// @param g Gltf object
	Model( Model&& g );

	/// Move assign a Gltf object
	/// @param g Gltf object
	Model& operator=( Model&& g );

	/// Delete copy constructor
	Model( const Model& ) = delete;

	/// Delete copy assignment
	Model& operator=( const Model& ) = delete;

	/// @return A new child node of the provided parent
	Handle<Node> create_node( const Handle<Node>& parent );

	/// @return The animation at that index, nullptr otherwise
	Accessor* get_accessor( size_t accessor );

	/// Load the nodes pointer using node indices
	void load_nodes();

	/// glTF asset
	Asset asset;

	/// Initializes asset
	/// @param j Json object describing the asset
	void init_asset( const nlohmann::json& j );

	/// Initializes buffers
	/// @param j Json object describing the buffers
	void init_buffers( const nlohmann::json& j );

	/// Initializes bufferViews
	/// @param j Json object describing the bufferViews
	void init_buffer_views( const nlohmann::json& j );

	/// Initializes cameras
	/// @param j Json object describing the cameras
	void init_cameras( const nlohmann::json& j );

	/// Initializes samplers
	/// @param j Json object describing the samplers
	void init_samplers( const nlohmann::json& j );

	/// Initializes images
	/// @param j Json object describing the images
	void init_images( const nlohmann::json& j );

	/// Initializes textures
	/// @param j Json object describing the textures
	void init_textures( const nlohmann::json& j );

	/// Initializes accessors
	/// @param j Json object describing the accessors
	void init_accessors( const nlohmann::json& j );

	/// Initializes materials
	/// @param j Json object describing the materials
	void init_materials( const nlohmann::json& j );

	/// Initializes meshes
	/// @param j Json object describing the meshes
	void init_meshes( const nlohmann::json& j );

	/// Initializes lights
	/// @param j Json object describing the lights
	void init_lights( const nlohmann::json& j );

	/// Initializes nodes
	/// @param j Json object describing the nodes
	void init_nodes( const nlohmann::json& j );

	/// Initializes animations
	/// @param j Json object describing the animations
	void init_animations( const nlohmann::json& j );

	/// Initializes shapes
	/// @param j Json object describing the shapes
	void init_shapes( const nlohmann::json& j );

	/// Initializes scripts
	/// @param j Json object describing scripts
	void init_scripts( const nlohmann::json& j );

	/// Initializes scenes
	/// @param j Json object describing the scenes
	void init_scenes( const nlohmann::json& j );

	/// Directory path of the gltf file
	std::string path;

	/// List of buffers
	Uvec<ByteBuffer> buffers;

	/// Cache of buffers
	std::map<const size_t, std::vector<char>> buffers_cache;

	/// List of buffer views
	Uvec<BufferView> buffer_views;

	/// List of cameras
	std::vector<GltfCamera> cameras;

	/// List of samplers
	Uvec<GltfSampler> samplers;

	/// List of images
	Uvec<Image> gltf_images;

	/// List of textures
	Uvec<Texture> textures;

	/// List of accessors
	Uvec<Accessor> accessors;

	/// List of materials
	Uvec<Material> materials;

	/// List of meshes
	Uvec<Mesh> meshes;

	/// List of lights
	Uvec<Light> lights;

	/// List of nodes
	Uvec<Node> nodes;

	/// List of animations
	Uvec<Animation> animations;

	/// List of shapes (abstract)
	Uvec<Rect> rects;
	Uvec<Box> boxes;
	Uvec<Sphere> spheres;
	Uvec<Bounds> bounds;

	/// List of scripts
	std::vector<Script> scripts;

	/// List of scenes
	std::vector<Scene> scenes;

	/// Current scene
	Scene* scene = nullptr;
};


template <typename T>
T from_string( const std::string& s );

template <>
Accessor::Type from_string<Accessor::Type>( const std::string& s );

template <>
Primitive::Semantic from_string<Primitive::Semantic>( const std::string& s );

template <>
Animation::Sampler::Interpolation from_string<Animation::Sampler::Interpolation>( const std::string& i );

template <>
Animation::Target::Path from_string<Animation::Target::Path>( const std::string& p );

template <>
Bounds::Type from_string<Bounds::Type>( const std::string& b );

template <typename T>
std::string to_string( const T& t );

template <>
std::string to_string<Accessor::Type>( const Accessor::Type& t );

template <>
std::string to_string<Primitive::Semantic>( const Primitive::Semantic& s );

template <>
std::string to_string<GltfSampler::Filter>( const GltfSampler::Filter& f );

template <>
std::string to_string<GltfSampler::Wrapping>( const GltfSampler::Wrapping& w );

template <>
std::string to_string<Primitive::Mode>( const Primitive::Mode& m );

}  // namespace spot::gfx
