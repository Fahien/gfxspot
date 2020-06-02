#pragma once

#include <cassert>
#include <variant>

#include <spot/handle.h>
#include <spot/math/math.h>
#include <spot/math/mat4.h>
#include <spot/math/shape.h>


namespace spot::gfx
{

class Node;
struct Rect;

struct Shape
{
	Shape()          = default;
	virtual ~Shape() = default;

	/// @brief Sets the node and its transform, in case the shape moves
	void set_node( Node& n );

	bool is_colliding_with( const Shape& s ) const;
	void add_collision( const Shape& s );
	void remove_collision( const Shape& s );

	virtual bool intersects( const Shape& s ) const
		{ assert( false && "unimplemented" ); return false; }

	virtual bool intersects( const Rect& s ) const
		{ assert( false && "unimplemented" ); return false; }

	virtual math::Vec2 distance( const Shape& s ) const
		{ assert( false && "unimplemented" ); return false; }

	virtual math::Vec2 distance( const Rect& s ) const
		{ assert( false && "unimplemented" ); return false; }


	virtual bool contains( const math::Vec2& p,
		const math::Mat4& transform = math::Mat4::Identity ) const
		{ assert( false && "unimplemented" ); return false; }

	Node* node = nullptr;

	/// @brief This is set by the owner node on updating transforms of the node hierarchy
	math::Mat4 matrix = math::Mat4::Identity;

	/// @brief Whether the shape is expected to move or not.
	/// A shape is static by default to achieve better performance.
	bool dynamic = false;

	/// @brief List of shapes currently colliding with this shape
	std::vector<const Shape*> collisions;

	std::function<void( const Shape& s, const Shape& o )> begin_colliding_with = {};
	std::function<void( const Shape& s, const Shape& o )> colliding_with = {};
	std::function<void( const Shape& s, const Shape& o )> end_colliding_with = {};

	/// @brief Callback which can be set to trigger some action
	std::function<void()> trigger = {};
};


struct Rect : public math::Rect, public Handled<Rect>, public Shape
{
	using math::Rect::Rect;
	
	bool intersects( const Shape& s ) const override;
	
	bool intersects( const Rect& r ) const override;
	
	math::Vec2 distance( const Shape& s ) const override;

	math::Vec2 distance( const Rect& s ) const override;

	bool contains( const math::Vec2& p,
		const math::Mat4& transform = math::Mat4::Identity ) const override;
};


struct Box : public math::Box, public Handled<Box>, public Shape
{
	using math::Box::Box;
};


struct Sphere : public math::Sphere, public Handled<Sphere>, public Shape
{
	using math::Sphere::Sphere;
};


struct Bounds : public Handled<Bounds>
{
	static bool intersects( const Node& a, const Node& b );

	static math::Vec2 distance( const Node& a, const Node& b );

	enum class Type
	{
		Undefined,
		Rect,
		Box,
		Sphere,
		Max,
	};

	bool is_colliding_with( const Bounds& b ) const;
	
	void add_collision( const Bounds& b );
	
	void remove_collision( const Bounds& b );

	bool intersects( const Bounds& b ) const;

	/// Whether the bounds are dynamic or not
	bool dynamic = false;

	Type type = Type::Rect;

	math::Rect shape;

	/// List of bounds currently colliding with this
	std::vector<const Bounds*> collisions;

	/// Collisions callback
	std::function<void( Node& self, Node& other )> begin_colliding_with;
	std::function<void( Node& self, Node& other )> colliding_with;
	std::function<void( Node& self, Node& other )> end_colliding_with;

	/// Callback which can be set to trigger some action
	std::function<void()> trigger;
};


} // namespace spot::gfx
