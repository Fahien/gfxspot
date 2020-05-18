#pragma once

#include <vector>


namespace spot::gfx
{


class Node;


/// @brief Collisions system
/// You first call update on the root of the scene
/// Then call resolve, @todo or call it within update?
class Collisions
{
  public:
	/// @brief This function visits recursively this node and its children
	/// while updating their shape's transforms. These shapes are then
	/// stored in the bounds vector member of this class
	void update( Node& node );

	/// @brief Main collisions logic
	/// Checks for collisions, calling their relative callbacks
	void resolve();

  private:
	/// @brief List of active nodes with bounds to check
	std::vector<Node*> nodes;
};


}  // namespace spot::gfx
