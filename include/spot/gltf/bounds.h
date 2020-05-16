#pragma once

#include <cassert>
#include <variant>

#include <spot/handle.h>
#include <spot/math/math.h>
#include <spot/math/shape.h>


namespace spot::gfx
{

class Gltf;
class Node;
struct Rect;

struct Shape
{
	Shape()          = default;
	virtual ~Shape() = default;

	/// @brief Sets a new transform matrix, in case the shape moves
	void set_matrix( const math::Mat4& m );

	bool is_colliding_with( const Shape& s ) const;
	void add_collision( const Shape& s );
	void remove_collision( const Shape& s );

	virtual bool intersects( const Shape& s,
		const math::Mat4& transform = math::Mat4::Identity ) const
		{ assert( false && "unimplemented" ); return false; }

	virtual bool intersects( const Rect& s,
		const math::Mat4& transform = math::Mat4::Identity ) const
		{ assert( false && "unimplemented" ); return false; }


	virtual bool contains( const math::Vec2& p,
		const math::Mat4& transform = math::Mat4::Identity ) const
		{ assert( false && "unimplemented" ); return false; }

	/// @todo This should be derived from node.absolute_transform()
	/// which recursively go up the tree to get parents' transforms
	math::Mat4 matrix = math::Mat4::Identity;

	std::vector<const Shape*> collisions = {};

	std::function<void(const Shape& s)> start_colliding_with = {};
	std::function<void(const Shape& s)> colliding_with = {};
	std::function<void(const Shape& s)> end_colliding_with = {};
};


struct Rect : public math::Rect, public Handled<Rect>, Shape
{
	using math::Rect::Rect;
	
	bool intersects( const Shape& s,
		const math::Mat4& transform = math::Mat4::Identity ) const override;
	
	bool intersects( const Rect& r,
		const math::Mat4& transform = math::Mat4::Identity ) const override;
	
	bool contains( const math::Vec2& p,
		const math::Mat4& transform = math::Mat4::Identity ) const override;
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

	std::variant<Handle<Rect>, Handle<Box>, Handle<Sphere>> clone_shape() const;

	Type type = Type::Rect;
	std::variant<Handle<Rect>, Handle<Box>, Handle<Sphere>> shape;
};


} // namespace spot::gfx
