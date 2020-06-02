#pragma once

#include <vector>


namespace spot::gfx
{


class Node;


/// @brief Collisions system
/// You first add the nodes of the scene, and then call update
class Collisions
{
  public:
	/// @brief This function visits recursively this node and its children
	/// to store them in the nodes vector member of this class
	void add( Node& node );

	/// @brief Main collisions logic
	/// Checks for collisions, calling their relative callbacks
	void update();

  private:
	/// @brief List of active nodes with bounds to check
	std::vector<Node*> nodes;
};


}  // namespace spot::gfx
