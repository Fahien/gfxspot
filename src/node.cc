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
	if ( parent )
	{
		return parent->get_absolute_matrix() * get_matrix();
	}
	return get_matrix();
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


Bounds* Node::get_bounds() const
{
	if ( bounds )
	{
		return &*bounds;
	}
	return nullptr;
}


}  // namespace spot::gfx
