#include "spot/gltf/node.h"

#include "spot/gltf/gltf.h"

namespace spot::gfx
{

Node::Node( const Handle<Mesh>& mesh )
: mesh { mesh }
{}


Node::Node( const std::string& name )
: name { name }
{}


Handle<Node> Scene::create_node( const std::string& name )
{
	auto node = model->nodes.push( Node( name ) );
	nodes.emplace_back( node );
	return node;
}


math::Mat4 Node::get_matrix() const
{
	auto transform = matrix;
	transform.scale( scale );
	transform.rotate( rotation );
	transform.translate( translation );
	return transform;
}


math::Mat4 Node::get_absolute_matrix() const
{
	if ( auto parent_node = get_parent() )
	{
		return parent_node->get_absolute_matrix() * get_matrix();
	}
	return get_matrix();
}


bool Node::contains( const math::Vec2& point ) const
{
	/// @todo Make this a bounding box and store an handle to it into a node
	auto rect = math::Rectangle();
	rect.width = 1.0f;
	rect.height = 1.0f;
	rect.x = -rect.width / 2.0f;
	rect.y = -rect.height / 2.0f;

	rect.x += translation.x;
	rect.y += translation.y;

	return rect.contains( point.x, point.y );
}


Handle<Node> Node::get_parent() const
{
	return parent;
}


Handle<Node> Node::create_child( const std::string& name )
{
	auto child = model->create_node( handle );
	child->name = name;
	return child;
}


void Node::add_child( const Handle<Node>& child )
{
	assert( child != handle && "Cannot add child to itself" );
	assert( !child->parent && "Cannot add a child which already has a parent" );
	child->parent = handle;
	children.emplace_back( child );
}


void Node::remove_from_parent()
{
	if ( parent )
	{
		// Remove node from parent's children
		auto it = std::find( std::begin( parent->children ), std::end( parent->children ), handle );
		if ( it != std::end( parent->children ) )
		{
			parent->children.erase( it );
		}

		parent = {};
	}
}


}  // namespace spot::gfx
