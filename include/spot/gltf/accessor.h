#pragma once

#include "spot/gltf/buffer.h"

namespace spot::gfx
{

/// @brief Typed view into a bufferView
struct Accessor
{
	/// Datatype of components in the attribute
	enum class ComponentType
	{
		BYTE           = 5120,
		UNSIGNED_BYTE  = 5121,
		SHORT          = 5122,
		UNSIGNED_SHORT = 5123,
		UNSIGNED_INT   = 5125,
		FLOAT          = 5126
	};

	/// Specifies if the attribute is a scalar, vector, or matrix
	enum class Type
	{
		NONE,
		SCALAR,
		VEC2,
		VEC3,
		VEC4,
		MAT2,
		MAT3,
		MAT4
	};

	/// @return The size of the data pointed by this accessor
	size_t get_size() const;

	/// @return The address of the data pointed by this accessor
	const uint8_t* get_data() const;

	/// @return The stride of the buffer view pointed by this accessor
	size_t get_stride() const;
	
	/// The model of the accessor
	Handle<Accessor> handle = {};

	/// @return The buffer view pointed by this accessor
	Handle<BufferView> buffer_view = {};

	/// Offset relative to the start of the bufferView in bytes
	size_t byte_offset = 0;

	/// Datatype of components in the attribute
	ComponentType component_type;

	/// Number of attributes referenced by this accessor
	size_t count;

	/// Specifies if the attribute is a scalar, vector, or matrix
	Type type;

	/// Maximum value of each component in this attribute
	std::vector<float> max;

	/// Minimum value of each component in this attribute
	std::vector<float> min;
};


} // namespace spot::gfx
