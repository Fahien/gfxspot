#pragma once

#include <cassert>
#include <spot/math/math.h>
#include <spot/math/shape.h>
#include "spot/gltf/node.h"

namespace spot::gfx
{

class Gltf;

struct Bounds
{
	enum class Type
	{
		Undefined,
		Rect,
		Box,
		Sphere,
		Max,
	};

	int32_t index = -1;
	Type type = Type::Rect;
	int32_t shape = -1;
};


/// @todo 
/// As it is really coupled with it
/// How about subclasses deriving from both this and math::ConcreteShapes?
struct Shape
{
	Shape()          = default;
	virtual ~Shape() = default;

	/// @brief Sets a new transform matrix, in case the shape moves
	void set_matrix( const math::Mat4& m );

	bool is_colliding_with( const Shape& s ) const;
	void add_collision( const Shape& s );
	void remove_collision( const Shape& s );

	virtual bool intersects( const Shape& s ) const { assert( false && "unimplemented" ); return false; }
	virtual bool contains( const math::Vec2& p ) const { assert( false && "unimplemented" ); return false; }

	Gltf* model = nullptr;

	/// @brief Index of the shape
	int32_t index = -1;

	/// @brief Node this shape belongs to
	Handle<Node> node = {};

	/// @todo This should be derived from node.absolute_transform()
	/// which recursively go up the tree to get parents' transforms
	math::Mat4 matrix = math::Mat4::identity;

	std::vector<const Shape*> collisions = {};

	std::function<void(const Shape& s)> start_colliding_with = {};
	std::function<void(const Shape& s)> colliding_with = {};
	std::function<void(const Shape& s)> end_colliding_with = {};
};


struct Rect : public math::Rect, Shape
{
	using math::Rect::Rect;

	bool contains( const math::Vec2& p ) const override;
};


struct Box : public math::Box, Shape
{
	using math::Box::Box;

};


struct Sphere : public math::Sphere, Shape
{
	using math::Sphere::Sphere;
};


} // namespace spot::gfx
