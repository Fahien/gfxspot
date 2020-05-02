#pragma once

#include <spot/gltf/color.h>
#include <spot/gltf/mesh.h>
#include <spot/gltf/node.h>
#include <spot/hash.h>
#include <spot/math/math.h>

namespace std
{
template <>
struct hash<spot::math::Vec2>
{
	size_t operator()(const spot::math::Vec2& vec) const { return std::hash_combine(vec.x, vec.y); }
};

template <>
struct hash<spot::math::Vec3>
{
	size_t operator()(const spot::math::Vec3& vec) const { return std::hash_combine(vec.x, vec.y, vec.z); }
};

template <>
struct hash<spot::gfx::Color>
{
	size_t operator()(const spot::gfx::Color& color) const { return std::hash_combine(color.r, color.g, color.b, color.a); }
};

template <>
struct hash<spot::gfx::Vertex>
{
	size_t operator()(const spot::gfx::Vertex& vert) const
	{
		auto hp = hash<spot::math::Vec3>()(vert.p);
		auto hc = hash<spot::gfx::Color>()(vert.c);
		auto ht = hash<spot::math::Vec2>()(vert.t);
		return std::hash_combine(hp, hc, ht);
	}
};

template <>
struct hash<spot::gfx::Primitive>
{
	size_t operator()(const spot::gfx::Primitive& pm) const
	{
		auto hp = std::hash<std::vector<spot::gfx::Vertex>>()(pm.vertices);
		auto hi = std::hash<std::vector<spot::gfx::Index>>()(pm.indices);
		return std::hash_combine(hp, hi);
	}
};

}  // namespace std
