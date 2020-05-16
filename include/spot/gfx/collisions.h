#pragma once

#include <unordered_map>
#include <vector>

#include <spot/handle.h>
#include <spot/math/math.h>


namespace spot::gfx
{


class Node;
class Bounds;


class Collisions
{
  public:
	/// @brief Updates collisions
	void update( const Handle<Node>& node, const math::Mat4& transform = math::Mat4::Identity );

	void resolve();

	std::vector<Handle<Bounds>> boundss;
};


}  // namespace spot::gfx
