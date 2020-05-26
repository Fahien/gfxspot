#include <fstream>
#include <stdexcept>
#include <spot/file/ifstream.h>

#include "spot/gfx/model.h"
#include "spot/gltf/node.h"


namespace spot::gfx
{


Model::Model( Model&& other )
: asset{ std::move( other.asset ) }
, path{ std::move( other.path ) }
, buffers_cache{ std::move( other.buffers_cache ) }
, cameras{ std::move( other.cameras ) }
, lights{ std::move( other.lights ) }
, scripts{ std::move( other.scripts ) }
, scenes{ std::move( other.scenes ) }
, scene{ std::move( other.scene ) }
{
	std::for_each( std::begin( scenes ), std::end( scenes ), [this]( auto& scene ) { scene.model = this; } );
	load_nodes();
}


Model& Model::operator=( Model&& other )
{
	asset         = std::move( other.asset );
	path          = std::move( other.path );
	buffers_cache = std::move( other.buffers_cache );
	cameras       = std::move( other.cameras );
	std::swap( lights, other.lights );
	scripts       = std::move( other.scripts );
	scenes        = std::move( other.scenes );
	scene         = std::move( other.scene );

	std::for_each( std::begin( scenes ), std::end( scenes ), [this]( auto& scene ) { scene.model = this; } );
	load_nodes();

	return *this;
}


nlohmann::json read_json( const std::string& path )
{
	// read a JSON file
	auto in = file::Ifstream( path );
	assert( in.is_open() && "Cannot open gltf file" );
	nlohmann::json js;
	in >> js;
	return js;
}


Model::Model( const std::string& path )
: Model( read_json( path ), path )
{}



Model::Model( const nlohmann::json& j, const std::string& pth )
{
	// Get the directory path
	auto index = pth.find_last_of( "/\\" );
	path = pth.substr( 0, index );

	// Asset
	init_asset( j["asset"] );

	// ByteBuffer
	if ( j.count( "buffers" ) )
	{
		init_buffers( j["buffers"] );
	}

	// BufferViews
	if ( j.count( "bufferViews" ) )
	{
		init_buffer_views( j["bufferViews"] );
	}

	// Cameras
	if ( j.count( "cameras" ) )
	{
		init_cameras( j["cameras"] );
	}

	// Samplers
	if ( j.count( "samplers" ) )
	{
		init_samplers( j["samplers"] );
	}

	// Images
	if ( j.count( "images" ) )
	{
		init_images( j["images"] );
	}

	// Textures
	if ( j.count( "textures" ) )
	{
		init_textures( j["textures"] );
	}

	// Accessors
	init_accessors( j["accessors"] );

	// Materials
	if ( j.count( "materials" ) )
	{
		init_materials( j["materials"] );
	}

	// Meshes
	if ( j.count( "meshes" ) )
	{
		init_meshes( j["meshes"] );
	}

	// Extras
	if ( j.count( "extras" ) )
	{
		auto& extras = j["extras"];

		// Scripts
		if ( extras.count( "scripts" ) )
		{
			init_scripts( extras["scripts"] );
		}

		// Shapes
		if ( extras.count( "shapes" ) )
		{
			init_shapes( extras["shapes"] );
		}
	}

	// Nodes
	if ( j.count( "nodes" ) )
	{
		init_nodes( j["nodes"] );
	}

	// Animations
	if ( j.count( "animations" ) )
	{
		init_animations( j["animations"] );
	}

	// Extensions
	if ( j.count( "extensions" ) )
	{
		auto extensions = j["extensions"];

		// Lights
		if ( extensions.count( "KHR_lights_punctual" ) )
		{
			init_lights( extensions["KHR_lights_punctual"]["lights"] );
		}
	}

	// Scenes
	if ( j.count( "scenes" ) )
	{
		init_scenes( j["scenes"] );

		size_t index = 0;

		if ( j.count( "scene" ) )
		{
			index = j["scene"].get<size_t>();
		}
		scene = &scenes[index];
	}
}


void Model::init_asset( const nlohmann::json& j )
{
	// Version (mandatory)
	asset.version = j["version"].get<std::string>();

	// Generator
	if ( j.count( "generator" ) )
	{
		asset.generator = j["generator"].get<std::string>();
	}

	// Copyright
	if ( j.count( "copyright" ) )
	{
		asset.copyright = j["copyright"].get<std::string>();
	}
}


void Model::init_buffers( const nlohmann::json& j )
{
	for ( const auto& b : j )
	{
		// ByteBuffer length in bytes (mandatory)
		auto byte_length = b["byteLength"].get<size_t>();

		// Uri of the binary file to upload
		std::string uri;
		if ( b.count( "uri" ) )
		{
			uri = b["uri"].get<std::string>();
			// If it is not data
			if ( uri.rfind( "data:", 0 ) != 0 )
			{
				uri = path + "/" + uri;
			}
		}

		buffers.push( ByteBuffer( uri, byte_length ) );
	}
}


void Model::init_buffer_views( const nlohmann::json& j )
{
	for ( const auto& v : j )
	{
		auto view = buffer_views.push();

		// ByteBuffer
		auto buffer_index = v["buffer"].get<size_t>();
		view->buffer = buffers.find( buffer_index );

		// Byte offset
		if ( v.count( "byteOffset" ) )
		{
			view->byte_offset = v["byteOffset"].get<size_t>();
		}

		// Byte length
		if ( v.count( "byteLength" ) )
		{
			view->byte_length = v["byteLength"].get<size_t>();
		}

		// Byte stride
		if ( v.count( "byteStride" ) )
		{
			view->byte_stride = v["byteStride"].get<size_t>();
		}

		// Target
		if ( v.count( "target" ) )
		{
			view->target = static_cast<BufferView::Target>( v["target"].get<size_t>() );
		}
	}
}


void Model::init_cameras( const nlohmann::json& j )
{
	for ( const auto& c : j )
	{
		GltfCamera camera;

		// Type
		auto type   = c["type"].get<std::string>();
		camera.type = ( type == "orthographic" ) ? GltfCamera::Type::Orthographic : GltfCamera::Type::Perspective;

		// Camera
		if ( camera.type == GltfCamera::Type::Orthographic )
		{
			camera.orthographic.xmag  = c["orthographic"]["xmag"].get<float>();
			camera.orthographic.ymag  = c["orthographic"]["ymag"].get<float>();
			camera.orthographic.zfar  = c["orthographic"]["zfar"].get<float>();
			camera.orthographic.znear = c["orthographic"]["znear"].get<float>();
		}
		else
		{
			auto& perspective = c["perspective"];
			if ( perspective.count( "aspectRatio" ) )
			{
				camera.perspective.aspect_ratio = perspective["aspectRatio"].get<float>();
			}
			camera.perspective.yfov  = c["perspective"]["yfov"].get<float>();
			camera.perspective.zfar  = c["perspective"]["zfar"].get<float>();
			camera.perspective.znear = c["perspective"]["znear"].get<float>();
		}

		// Name
		if ( c.count( "name" ) )
		{
			camera.name = c["name"].get<std::string>();
		}

		cameras.push_back( std::move( camera ) );
	}
}


template <>
std::string to_string<GltfSampler::Filter>( const GltfSampler::Filter& f )
{
	switch ( f )
	{
		case GltfSampler::Filter::NONE:
			return "NONE";
		case GltfSampler::Filter::NEAREST:
			return "NEAREST";
		case GltfSampler::Filter::LINEAR:
			return "LINEAR";
		case GltfSampler::Filter::NEAREST_MIPMAP_NEAREST:
			return "NEAREST_MIPMAP_NEAREST";
		case GltfSampler::Filter::LINEAR_MIPMAP_NEAREST:
			return "LINEAR_MIPMAP_NEAREST";
		case GltfSampler::Filter::NEAREST_MIPMAP_LINEAR:
			return "NEAREST_MIPMAP_LINEAR";
		case GltfSampler::Filter::LINEAR_MIPMAP_LINEAR:
			return "LINEAR_MIPMAP_LINEAR";
		default:
			return "UNDEFINED";
	}
}


template <>
std::string to_string<GltfSampler::Wrapping>( const GltfSampler::Wrapping& w )
{
	switch ( w )
	{
		case GltfSampler::Wrapping::CLAMP_TO_EDGE:
			return "CLAMP_TO_EDGE";
		case GltfSampler::Wrapping::MIRRORED_REPEAT:
			return "MIRRORED_REPEAT";
		case GltfSampler::Wrapping::REPEAT:
			return "REPEAT";
		default:
			return "UNDEFINED";
	}
}


template <>
std::string to_string<Primitive::Mode>( const Primitive::Mode& m )
{
	switch ( m )
	{
		case Primitive::Mode::POINTS:
			return "Points";
		case Primitive::Mode::LINES:
			return "Lines";
		case Primitive::Mode::LINE_LOOP:
			return "LineLoop";
		case Primitive::Mode::LINE_STRIP:
			return "LineStrip";
		case Primitive::Mode::TRIANGLES:
			return "Triangles";
		case Primitive::Mode::TRIANGLE_STRIP:
			return "TriangleStrip";
		case Primitive::Mode::TRIANGLE_FAN:
			return "TriangleFan";
		default:
			return "Undefined";
	}
}

void Model::init_samplers( const nlohmann::json& j )
{
	for ( const auto& s : j )
	{
		GltfSampler sampler;

		// Mag Filter
		if ( s.count( "magFilter" ) )
		{
			sampler.magFilter = static_cast<GltfSampler::Filter>( s["magFilter"].get<int>() );
		}

		// Min Filter
		if ( s.count( "minFilter" ) )
		{
			sampler.minFilter = static_cast<GltfSampler::Filter>( s["minFilter"].get<int>() );
		}

		// WrapS
		if ( s.count( "wrapS" ) )
		{
			sampler.wrapS = static_cast<GltfSampler::Wrapping>( s["wrapS"].get<int>() );
		}

		// WrapT
		if ( s.count( "wrapT" ) )
		{
			sampler.wrapT = static_cast<GltfSampler::Wrapping>( s["wrapT"].get<int>() );
		}

		// Name
		if ( s.count( "name" ) )
		{
			sampler.name = s["name"].get<std::string>();
		}

		samplers->push_back( sampler );
	}
}


void Model::init_images( const nlohmann::json& j )
{
	for ( const auto& i : j )
	{
		auto image = gltf_images.push();

		if ( i.count( "uri" ) )
		{
			image->uri = path + "/" + i["uri"].get<std::string>();
		}

		if ( i.count( "mimeType" ) )
		{
			image->mime_type = i["mimeType"].get<std::string>();
		}

		if ( i.count( "bufferView" ) )
		{
			image->buffer_view = i["bufferView"].get<uint32_t>();
		}

		if ( i.count( "name" ) )
		{
			image->name = i["name"].get<std::string>();
		}
	}
}


void Model::init_textures( const nlohmann::json& j )
{
	for ( const auto& t : j )
	{
		auto texture = textures.push();

		// GltfSampler
		if ( t.count( "sampler" ) )
		{
			auto index = t["sampler"].get<size_t>();
			texture->sampler = samplers.find( index );
		}

		// Image
		if ( t.count( "source" ) )
		{
			auto index = t["source"].get<int32_t>();
			texture->source = gltf_images.find( index );
		}

		// Name
		if ( t.count( "name" ) )
		{
			texture->name = t["name"].get<std::string>();
		}
	}
}


template <>
Accessor::Type from_string<Accessor::Type>( const std::string& s )
{
	if ( s == "SCALAR" )
	{
		return Accessor::Type::SCALAR;
	}
	else if ( s == "VEC2" )
	{
		return Accessor::Type::VEC2;
	}
	else if ( s == "VEC3" )
	{
		return Accessor::Type::VEC3;
	}
	else if ( s == "VEC4" )
	{
		return Accessor::Type::VEC4;
	}
	else if ( s == "MAT2" )
	{
		return Accessor::Type::MAT2;
	}
	else if ( s == "MAT3" )
	{
		return Accessor::Type::MAT3;
	}
	else if ( s == "MAT4" )
	{
		return Accessor::Type::MAT4;
	}
	else
	{
		assert( false );
		return Accessor::Type::NONE;
	}
}


template <>
std::string to_string<Accessor::Type>( const Accessor::Type& t )
{
	if ( t == Accessor::Type::SCALAR )
	{
		return "SCALAR";
	}
	else if ( t == Accessor::Type::VEC2 )
	{
		return "VEC2";
	}
	else if ( t == Accessor::Type::VEC3 )
	{
		return "VEC3";
	}
	else if ( t == Accessor::Type::VEC4 )
	{
		return "VEC4";
	}
	else if ( t == Accessor::Type::MAT2 )
	{
		return "MAT2";
	}
	else if ( t == Accessor::Type::MAT3 )
	{
		return "MAT3";
	}
	else if ( t == Accessor::Type::MAT4 )
	{
		return "MAT4";
	}
	else
	{
		assert( false );
		return "NONE";
	}
}



size_t size_of( Accessor::ComponentType ct )
{
	switch ( ct )
	{
	case Accessor::ComponentType::BYTE:          return sizeof( uint8_t );
	case Accessor::ComponentType::UNSIGNED_BYTE: return sizeof( uint8_t );
	case Accessor::ComponentType::SHORT:         return sizeof( uint16_t );
	case Accessor::ComponentType::UNSIGNED_SHORT:return sizeof( uint16_t );
	case Accessor::ComponentType::UNSIGNED_INT:  return sizeof( uint32_t );
	case Accessor::ComponentType::FLOAT:         return sizeof( float );
	default: assert( false && "Invalid accessor component type" ); return 0;
	}
}


size_t size_of( Accessor::Type tp )
{
	switch ( tp )
	{
	case Accessor::Type::NONE: return 1;
	case Accessor::Type::SCALAR: return 1;
	case Accessor::Type::VEC2: return 2;
	case Accessor::Type::VEC3: return 3;
	case Accessor::Type::VEC4: return 4;
	case Accessor::Type::MAT2: return 4;
	case Accessor::Type::MAT3: return 9;
	case Accessor::Type::MAT4: return 16;
	default: assert( false && "Invalid accessor type" ); return 0;
	}
}

size_t Accessor::get_size() const
{
	return count * size_of( component_type ) * size_of( type );
}


const uint8_t* Accessor::get_data() const
{
	auto& buffer = buffer_view->buffer;
	auto data = buffer->data.data() + buffer_view->byte_offset + byte_offset;
	return reinterpret_cast<const uint8_t*>( data );
}


size_t Accessor::get_stride() const
{
	return buffer_view->byte_stride;
}


void Model::init_accessors( const nlohmann::json& j )
{
	for ( const auto& a : j )
	{
		auto accessor = accessors.push();

		// ByteBuffer view
		if ( a.count( "bufferView" ) )
		{
			auto buffer_view_index = a["bufferView"].get<size_t>();
			accessor->buffer_view = buffer_views.find( buffer_view_index );
		}

		// Byte offset
		if ( a.count( "byteOffset" ) )
		{
			accessor->byte_offset = a["byteOffset"].get<size_t>();
		}

		// Component type
		accessor->component_type = a["componentType"].get<Accessor::ComponentType>();

		// Count
		accessor->count = a["count"].get<size_t>();

		// Type
		accessor->type = from_string<Accessor::Type>( a["type"].get<std::string>() );

		// Max
		if ( a.count( "max" ) )
		{
			for ( const auto& value : a["max"] )
			{
				accessor->max.push_back( value.get<float>() );
			}
		}

		// Min
		if ( a.count( "min" ) )
		{
			for ( const auto& value : a["min"] )
			{
				accessor->min.push_back( value.get<float>() );
			}
		}
	}
}


void Model::init_materials( const nlohmann::json& j )
{
	for ( const auto& m : j )
	{
		auto material = materials.push();

		// Name
		if ( m.count( "name" ) )
		{
			material->name = m["name"].get<std::string>();
		}

		// PbrMetallicRoughness
		if ( m.count( "pbrMetallicRoughness" ) )
		{
			auto& mr = m["pbrMetallicRoughness"];

			if ( mr.count( "baseColorFactor" ) )
			{
				auto color = mr["baseColorFactor"].get<std::vector<float>>();
				material->pbr.color.r = color[0];
				material->pbr.color.g = color[1];
				material->pbr.color.b = color[2];
				material->pbr.color.a = color[3];
			}

			if ( mr.count( "baseColorTexture" ) )
			{
				auto index = mr["baseColorTexture"]["index"].get<size_t>();
				material->texture_handle = textures.find( index );
			}

			if ( mr.count( "metallicFactor" ) )
			{
				material->pbr.metallic = mr["metallicFactor"].get<float>();
			}

			if ( mr.count( "roughnessFactor" ) )
			{
				material->pbr.roughness = mr["roughnessFactor"].get<float>();
			}
		}
	}
}


template <>
Primitive::Semantic from_string<Primitive::Semantic>( const std::string& s )
{
	if ( s == "POSITION" )
	{
		return Primitive::Semantic::POSITION;
	}
	else if ( s == "NORMAL" )
	{
		return Primitive::Semantic::NORMAL;
	}
	else if ( s == "TANGENT" )
	{
		return Primitive::Semantic::TANGENT;
	}
	else if ( s == "TEXCOORD_0" )
	{
		return Primitive::Semantic::TEXCOORD_0;
	}
	else if ( s == "TEXCOORD_1" )
	{
		return Primitive::Semantic::TEXCOORD_1;
	}
	else if ( s == "COLOR_0" )
	{
		return Primitive::Semantic::COLOR_0;
	}
	else if ( s == "JOINTS_0" )
	{
		return Primitive::Semantic::JOINTS_0;
	}
	else if ( s == "WEIGHTS_0" )
	{
		return Primitive::Semantic::WEIGHTS_0;
	}
	else
	{
		assert( false );
		return Primitive::Semantic::NONE;
	}
}


template <>
std::string to_string<Primitive::Semantic>( const Primitive::Semantic& s )
{
	if ( s == Primitive::Semantic::POSITION )
	{
		return "Position";
	}
	else if ( s == Primitive::Semantic::NORMAL )
	{
		return "Normal";
	}
	else if ( s == Primitive::Semantic::TANGENT )
	{
		return "Tangent";
	}
	else if ( s == Primitive::Semantic::TEXCOORD_0 )
	{
		return "Texcoord0";
	}
	else if ( s == Primitive::Semantic::TEXCOORD_1 )
	{
		return "Texcoord1";
	}
	else if ( s == Primitive::Semantic::COLOR_0 )
	{
		return "Color0";
	}
	else if ( s == Primitive::Semantic::JOINTS_0 )
	{
		return "Joints0";
	}
	else if ( s == Primitive::Semantic::WEIGHTS_0 )
	{
		return "Weights0";
	}
	else
	{
		assert( false );
		return "None";
	}
}


void Model::init_meshes( const nlohmann::json& j )
{
	for ( const auto& m : j )
	{
		Mesh mesh;

		// Name
		if ( m.count( "name" ) )
		{
			mesh.name = m["name"].get<std::string>();
		}

		// Primitives
		for ( const auto& p : m["primitives"] )
		{
			auto& primitive = mesh.primitives.emplace_back();

			auto attributes = p["attributes"].get<std::map<std::string, unsigned>>();

			for ( const auto& a : attributes )
			{
				auto semantic = from_string<Primitive::Semantic>( a.first );
				auto accessor = accessors.find( a.second );
				primitive.attributes.emplace( semantic, accessor );
			}

			if ( p.count( "indices" ) )
			{
				auto indices_index = p["indices"].get<int32_t>();
				primitive.indices_handle = accessors.find( indices_index );
			}

			if ( p.count( "material" ) )
			{
				auto material_index = p["material"].get<int32_t>();
				primitive.material = materials.find( material_index );
			}

			if ( p.count( "mode" ) )
			{
				primitive.mode = p["mode"].get<Primitive::Mode>();
			}
		}

		meshes->push_back( std::move( mesh ) );
	}
}


void Model::init_lights( const nlohmann::json& j )
{
	for ( const auto& l : j )
	{
		auto light = lights.push();

		// Name
		if ( l.count( "name" ) )
		{
			light->name = l["name"].get<std::string>();
		}

		// Color
		if ( l.count( "color" ) )
		{
			auto color = l["color"].get<std::vector<float>>();
			light->color.set( color[0], color[1], color[2] );
		}

		// Intensity
		if ( l.count( "intensity" ) )
		{
			light->intensity = l["intensity"].get<float>();
		}

		// Range
		if ( l.count( "range" ) )
		{
			light->range = l["range"].get<float>();
		}

		// Type
		if ( l.count( "type" ) )
		{
			auto type = l["type"].get<std::string>();
			if ( type == "point" )
			{
				light->type = Light::Type::Point;
			}
			else if ( type == "directional" )
			{
				light->type = Light::Type::Directional;
			}
			else if ( type == "spot" )
			{
				light->type = Light::Type::Spot;

				if ( l.count( "spot" ) )
				{
					const auto& spot = l["spot"];
					if ( spot.count( "innerConeAngle" ) )
					{
						light->spot.inner_cone_angle = l["spot"]["innerConeAngle"].get<float>();
					}
					if ( spot.count( "innerConeAngle" ) )
					{
						light->spot.outer_cone_angle = l["spot"]["outerConeAngle"].get<float>();
					}
				}
			}
			else
			{
				assert( false && "Invalid light type" );
			}
		}
	}
}


void Model::init_nodes( const nlohmann::json& j )
{
	size_t i = 0;

	for ( const auto& n : j )
	{
		auto node = nodes.push();

		// Name
		if ( n.count( "name" ) )
		{
			node->name = n["name"].get<std::string>();
		}

		// Camera
		if ( n.count( "camera" ) )
		{
			unsigned m  = n["camera"];
			node->camera = &( cameras[m] );
		}

		// Matrix
		if ( n.count( "matrix" ) )
		{
			auto marr = n["matrix"].get<std::array<float, 16>>();
			node->matrix = math::Mat4( marr.data() );
		}

		// Mesh
		if ( n.count( "mesh" ) )
		{
			auto mesh_index = n["mesh"];
			node->mesh = meshes.find( mesh_index );
		}

		// Rotation
		if ( n.count( "rotation" ) )
		{
			auto qvec = n["rotation"].get<std::vector<float>>();
			node->set_rotation( { qvec[3], qvec[0], qvec[1], qvec[2] } );
		}

		// Scale
		if ( n.count( "scale" ) )
		{
			auto s = n["scale"].get<std::vector<float>>();
			node->set_scaling( { s[0], s[1], s[2] } );
		}

		// Translation
		if ( n.count( "translation" ) )
		{
			auto t = n["translation"].get<std::vector<float>>();
			node->set_translation( { t[0], t[1], t[2] } );
		}

		// Estensions
		if ( n.count( "extensions" ) )
		{
			auto& extensions = n["extensions"];
			// Lights
			if ( extensions.count( "KHR_lights_punctual" ) )
			{
				auto light_index = extensions["KHR_lights_punctual"]["light"].get<size_t>();
				node->light = lights.find( light_index );
			}
		}

		// Extras
		if ( n.count( "extras" ) )
		{
			auto& extras = n["extras"];

			// Bounds
			if ( extras.count( "bounds" ) )
			{
				auto bounds_index = extras["bounds"].get<size_t>();
				/// @todo Fix this
				// node->bounds = bounds.find( bounds_index );
			}

			// Scripts
			if ( extras.count( "scripts" ) )
			{
				node->scripts_indices = extras["scripts"].get<std::vector<size_t>>();
			}
		}
	}

	// Second pass
	i = 0;
	for ( auto& n : j )
	{
		auto& node = (*nodes)[i];

		// Now children are available
		if ( n.count( "children" ) )
		{
			auto handles = n["children"].get<std::vector<size_t>>();
			node.children.resize( handles.size() );
			for ( size_t i = 0; i < handles.size(); ++i )
			{
				node.children[i] = nodes.find( handles[i] );
			}
		}

		++i;
	}
}


template <>
Animation::Sampler::Interpolation from_string<Animation::Sampler::Interpolation>( const std::string& i )
{
	if ( i == "LINEAR" )
	{
		return Animation::Sampler::Interpolation::Linear;
	}
	else if ( i == "STEP" )
	{
		return Animation::Sampler::Interpolation::Step;
	}
	else if ( i == "CUBICSPLINE" )
	{
		return Animation::Sampler::Interpolation::Cubicspline;
	}
	assert( false );
	return Animation::Sampler::Interpolation::Linear;
}


template <>
Animation::Target::Path from_string<Animation::Target::Path>( const std::string& p )
{
	if ( p == "translation" )
	{
		return Animation::Target::Path::Translation;
	}
	else if ( p == "rotation" )
	{
		return Animation::Target::Path::Rotation;
	}
	else if ( p == "scale" )
	{
		return Animation::Target::Path::Scale;
	}
	else if ( p == "weights" )
	{
		return Animation::Target::Path::Weights;
	}
	assert( false );
	return Animation::Target::Path::None;
}


void Model::init_animations( const nlohmann::json& j )
{
	for ( auto& a : j )
	{
		auto animation = Animation( handle );

		if ( a.count( "name" ) )
		{
			animation.name = a["name"].get<std::string>();
		}

		for ( auto& s : a["samplers"] )
		{
			Animation::Sampler sampler;

			auto input  = s["input"].get<size_t>();
			sampler.input = accessors.find( input );

			auto output = s["output"].get<size_t>();
			sampler.output = accessors.find( output );

			if ( s.count( "interpolation" ) )
			{
				sampler.interpolation =
				    from_string<Animation::Sampler::Interpolation>( s["interpolation"].get<std::string>() );
			}

			animation.samplers->push_back( std::move( sampler ) );
		}

		for ( auto& c : a["channels"] )
		{
			Animation::Channel channel;

			auto index = c["sampler"].get<size_t>();
			channel.sampler = animation.samplers.find( index );

			// Target
			auto& t = c["target"];

			if ( t.count( "node" ) )
			{
				auto index = t["node"].get<size_t>();
				channel.target.node = nodes.find( index );
			}

			channel.target.path = from_string<Animation::Target::Path>( t["path"].get<std::string>() );

			animation.channels->push_back( std::move( channel ) );
		}

		animations.push( std::move( animation ) );
	}
}


template <>
Bounds::Type from_string<Bounds::Type>( const std::string& b )
{
	if ( b == "box" )
	{
		return Bounds::Type::Box;
	}
	else if ( b == "sphere" )
	{
		return Bounds::Type::Sphere;
	}
	else
	{
		throw std::runtime_error{ "Bounds not valid: " + b };
	}
}


void Model::init_shapes( const nlohmann::json& ss )
{
	for ( auto& s : ss )
	{
		auto type = s["type"].get<std::string>();
		if ( type == "box" )
		{
			auto aa = s["box"]["a"].get<std::vector<float>>();
			auto a  = math::Vec3{ aa[0], aa[1], aa[2] };
			auto bb = s["box"]["b"].get<std::vector<float>>();
			auto b  = math::Vec3{ bb[0], bb[1], bb[2] };

			boxes.push( Box{ a, b } );
		}
		else if ( type == "sphere" )
		{
			auto oo = s["sphere"]["o"].get<std::vector<float>>();
			auto o  = math::Vec3{ oo[0], oo[1], oo[2] };

			auto r = s["sphere"]["r"].get<float>();

			spheres.push( Sphere{ o, r } );
		}
		else
		{
			throw std::runtime_error{ "Type not supported: " + type };
		}
	}
}


void Model::init_scripts( const nlohmann::json& ss )
{
	// Init scripts
	Script script;

	for ( auto& s : ss )
	{
		script.uri = s["uri"].get<std::string>();

		if ( s.count( "name" ) )
		{
			script.name = s["name"].get<std::string>();
		}
		else
		{
			script.name = script.uri;
		}

		scripts.push_back( script );
	}
}


void Model::load_nodes()
{
	for ( auto& node : *nodes )
	{
		// Solve parents
		for ( auto child : node.children )
		{
			child->parent = node.handle;
		}

		// Solve node script
		node.scripts.clear();

		if ( !node.scripts_indices.empty() )
		{
			for ( auto script_index : node.scripts_indices )
			{
				auto script = &scripts[script_index];
				node.scripts.push_back( script );
			}
		}
	}
}


Handle<Node> Model::create_node( const Handle<Node>& parent )
{
	auto node = nodes.push();
	parent->add_child( node );
	return node;
}


Accessor* Model::get_accessor( const size_t accessor )
{
	if ( accessor < accessors->size() )
	{
		return &(*accessors)[accessor];
	}
	return nullptr;
}


void Model::init_scenes( const nlohmann::json& j )
{
	for ( const auto& s : j )
	{
		Scene scene;
		scene.model = this;

		// Name
		if ( s.count( "name" ) )
		{
			scene.name = s["name"].get<std::string>();
		}

		// Nodes
		if ( s.count( "nodes" ) )
		{
			auto indices = s["nodes"].get<std::vector<size_t>>();
			scene.nodes.resize( indices.size() );
			for ( size_t i = 0; i < indices.size(); ++i )
			{
				scene.nodes[i] = nodes.find( indices[i] );
			}
		}

		scenes.push_back( scene );
	}

	load_nodes();
}

}  // namespace spot::gfx
