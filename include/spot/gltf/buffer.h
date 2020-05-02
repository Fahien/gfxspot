#pragma once

#include <string>
#include <vector>

#include "spot/handle.h"


namespace spot::gfx
{


/// Buffer pointing to binary geometry, animation, or skins
struct ByteBuffer : public Handled<ByteBuffer>
{
	ByteBuffer() = default;

	ByteBuffer( std::string uri, size_t byte_length );

	Handle<ByteBuffer> handle = {};

	/// Uri of the buffer
	std::string uri;

	/// Length of the buffer in bytes
	size_t byte_length = 0;

	/// Bytes lazily loaded
	std::vector<char> data;
};


/// View into a buffer, generally representing a subset of the buffer
struct BufferView : public Handled<BufferView>
{
	/// Target that the GPU buffer should be bound to
	enum class Target
	{
		None,
		ArrayBuffer        = 34962,
		ElementArrayBuffer = 34963
	};

	Handle<BufferView> handle = {};

	/// Index of the buffer
	Handle<ByteBuffer> buffer = {};

	/// Offset into the buffer in bytes
	size_t byte_offset = 0;

	/// Length of the bufferView in bytes
	size_t byte_length = 0;

	/// Stride, in bytes
	size_t byte_stride = 0;

	/// Target that the GPU buffer should be bound to
	Target target = Target::None;
};


} // namespace spot::gfx
