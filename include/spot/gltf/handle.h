#pragma once

#include <vector>
#include <memory>
#include <limits>

namespace spot::gfx
{


template <typename T>
class Uvec;


/// @brief The handle class is used to solve the problem of using
/// pointers to elements of a vector which can become dangling when
/// the vector gets resized and the element gets moved somwhere else.
/// The handle stores a reference of that vector and the index of the
/// elements it should point to. Using the index means that elements
/// of the vector should never be erased, or a handle might end up
/// referring to a different element.
template <typename T>
class Handle
{
  public:
	/// @brief Constucts an invalid handle
	Handle() = default;

	/// @param v Unique pointer to a vector ensures that vector would not move
	/// @param i Index to the element pointed to by the handle
	Handle( const Uvec<T>& v, size_t i )
	: vec { v.get() }
	, index { i }
	{
		assert( *this && "Handle is not valid" );
	}

	/// @return Whether the handle is valid or not
	explicit operator bool() const
	{
		return vec && index < vec->size();
	}

	T* operator->() const { return &**this; }
	T& operator*() const { assert( *this && "Handle is not valid" ); return ( *vec )[index]; }

	bool operator==( const Handle<T>& other ) const { return vec == other.vec && index == other.index; }
	bool operator!=( const Handle<T>& other ) const { return !( *this == other ); }

	size_t get_index() const { return index; }

  private:
	size_t index = std::numeric_limits<size_t>::max();
	std::vector<T>* vec = nullptr;
};


template <typename T>
class Uvec : public std::unique_ptr<std::vector<T>>
{
  public:
	Uvec() : std::unique_ptr<std::vector<T>>( std::make_unique<std::vector<T>>() ) {}

	Handle<T> push( T&& elem = {} );

	Handle<T> get_handle( size_t i ) const;

};


template <typename T>
Handle<T> Uvec<T>::push( T&& elem )
{
	auto vec = this->get();
	auto& ret = vec->emplace_back( std::move( elem ) );
	ret.handle = Handle<T>( *this, vec->size() - 1 );
	return ret.handle;
}


template <typename T>
Handle<T> Uvec<T>::get_handle( size_t index ) const
{
	return Handle<T>( *this, index );
}


template <typename T>
class Handled
{
  public:
	virtual ~Handled() = default;

	Handle<T> handle;
};


} // namespace spot::gfx


namespace std
{


template <typename T>
struct hash<spot::gfx::Handle<T>>
{
	size_t operator()( const spot::gfx::Handle<T>& handle ) const
	{
		return handle.get_index();
	}
};


} // namespace std
