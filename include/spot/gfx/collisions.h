#pragma once

#include <unordered_map>
#include <vector>

#include <spot/math/math.h>


namespace spot::gfx
{


class Node;
class Bounds;


class Collisions
{
  public:
	/// @brief Updates collisions
	void update( const Node& node, const math::Mat4& transform = math::Mat4::Identity );

	void resolve();

	std::vector<Bounds*> boundss;
};


}  // namespace spot::gfx
