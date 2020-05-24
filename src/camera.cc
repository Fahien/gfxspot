#include "spot/gfx/camera.h"

#include <cmath>

namespace spot::gfx
{


Camera::Camera()
{
	look_at( math::Vec3::Z, math::Vec3::Zero, math::Vec3::Y );
	set_orthographic( 2.0f, 2.0f, 0.125f, 8.0f );
	node.set_translation( math::Vec2( -1.0f, -1.0f ) );
}


void Camera::update( const VkViewport& viewport )
{
	switch ( type )
	{
	case Type::Perspective: set_perspective( viewport, perspective.yfov ); break;
	case Type::Orthographic: set_orthographic( viewport ); break;
	}
}


void Camera::set_orthographic( const VkViewport& viewport )
{
	// Put node camera in the middle
	const float x = viewport.x + viewport.width / 2.0f;
	const float y = viewport.y + viewport.height / 2.0f;
	node.set_translation( math::Vec2( x, y ) );

	set_orthographic( viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth );
}


void Camera::set_orthographic( float xmag, float ymag, float near, float far )
{
	type = Type::Orthographic;
	orthographic.xmag = xmag;
	orthographic.ymag = ymag;
	orthographic.zfar = far;
	orthographic.znear = near;

	math::Vec3 scale;
	scale.x = 2.0f / orthographic.xmag;
	scale.y = 2.0f / orthographic.ymag;
	scale.z = 1.0f / ( orthographic.znear - orthographic.zfar );

	proj = math::Mat4::Identity;
	proj.matrix[14] = ( orthographic.znear ) / ( orthographic.znear - orthographic.zfar );
	proj.matrix[0] = scale.x;
	proj.matrix[5] = -scale.y; /// @todo Understand why
	proj.matrix[10] = scale.z;
}


void Camera::set_perspective( const VkViewport& viewport, const float yfov )
{
	const float aspect_ratio = viewport.width / viewport.height;
	set_perspective( aspect_ratio, yfov, viewport.maxDepth, viewport.minDepth );
}


void Camera::set_perspective( const float a, const float y, const float f, const float n )
{
	assert( f > n && "Far should be greater than near" );
	type = Type::Perspective;
	perspective.aspect_ratio = a;
	perspective.yfov = y;
	perspective.zfar = f;
	perspective.znear = n;

	// Calculate projection matrix
	proj = math::Mat4::Identity;

	float cotfov = 1.0f / std::tan( 0.5f * y );
	proj.matrix[0] = cotfov / a;
	proj.matrix[5] = -cotfov;
	proj.matrix[10] = f / ( ( n - f ) * 2.0f );
	proj.matrix[11] = -1.0f;
	proj.matrix[14] = n * f / ( ( n - f ) * 2.0f );
}


void Camera::look_at( const math::Vec3& eye, const math::Vec3& center, const math::Vec3& up_arg )
{
	node.set_translation( eye );

	up = up_arg;

	forward = eye - center;
	forward.normalize();

	right = math::Vec3::cross( up, forward );
	right.normalize();

	up = math::Vec3::cross( forward, right );
	up.normalize();
}


math::Mat4 Camera::get_view() const
{
	math::Mat4 view;

	view( 0, 0 ) = right.x;
	view( 0, 1 ) = right.y;
	view( 0, 2 ) = right.z;
	view( 0, 3 ) = -math::Vec3::dot( right, node.get_translation() );
	view( 1, 0 ) = up.x;
	view( 1, 1 ) = up.y;
	view( 1, 2 ) = up.z;
	view( 1, 3 ) = -math::Vec3::dot( up, node.get_translation() );
	view( 2, 0 ) = forward.x;
	view( 2, 1 ) = forward.y;
	view( 2, 2 ) = forward.z;
	view( 2, 3 ) = -math::Vec3::dot( forward, node.get_translation() );
	view( 3, 0 ) = 0.0f;
	view( 3, 1 ) = 0.0f;
	view( 3, 2 ) = 0.0f;
	view( 3, 3 ) = 1.0f;

	return view;
}


} // namespace spot::gfx
