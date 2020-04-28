#pragma once

#include <cassert>
#include <variant>

#include <spot/math/math.h>
#include <spot/math/shape.h>

#include "spot/gltf/node.h"

namespace spot::gfx
{

class Gltf;


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


struct Rect : public math::Rect, public Handled<Rect>, Shape
{
	using math::Rect::Rect;

	bool contains( const math::Vec2& p ) const override;
};


struct Box : public math::Box, public Handled<Box>, Shape
{
	using math::Box::Box;

};


struct Sphere : public math::Sphere, public Handled<Sphere>, Shape
{
	using math::Sphere::Sphere;
};


struct Bounds : public Handled<Bounds>
{
	enum class Type
	{
		Undefined,
		Rect,
		Box,
		Sphere,
		Max,
	};

	Shape& get_shape() const;

	Type type = Type::Rect;
	std::variant<Handle<Rect>, Handle<Box>, Handle<Sphere>> shape;
};


} // namespace spot::gfx
