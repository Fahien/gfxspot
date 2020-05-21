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


void Node::invalidate()
{
	Handled<Node>::invalidate();
	if ( rect )
	{
		rect->invalidate();
	}
}


Node& Node::translate( const math::Vec3& t )
{
	dirty = true;
	translation += t;
	return *this;
}


Node& Node::rotate( const math::Quat& r )
{
	dirty = true;
	rotation *= r;
	return *this;
}


Node& Node::scale( const math::Vec3& s )
{
	dirty = true;
	scaling *= s;
	return *this;
}


Handle<Node> Scene::create_node( const std::string& name )
{
	auto node = model->nodes.push( Node( name ) );
	nodes.emplace_back( node );
	return node;
}


const math::Mat4& Node::get_transform() const
{
	return matrix;
}


void Node::update_transforms( const math::Mat4& transform )
{
	if ( dirty )
	{
		recalculate( transform );
	}
	else
	{
		for ( auto& child : children )
		{
			child->update_transforms( matrix );
		}
	}
}


void Node::recalculate( math::Mat4 transform )
{
	dirty = false;
	matrix = transform.scale( scaling ).rotate( rotation ).translate( translation );
	for ( auto& child : children )
	{
		child->recalculate( matrix );
	}
}


math::Mat4 Node::get_absolute_matrix() const
{
	if ( parent )
	{
		return parent->get_absolute_matrix() * matrix;
	}

	return matrix;
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
