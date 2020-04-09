#include "spot/gfx/camera.h"

#include <cmath>

namespace spot::gfx
{


Camera::Camera( const VkViewport& viewport )
{
	look_at( math::Vec3::Zero, math::Vec3::Zero, math::Vec3::Y );
	orthographic( viewport, 0.125f, 2.0f );
}


void Camera::orthographic( const VkViewport& viewport, float near, float far )
{
	orthographic(
		viewport.x, viewport.width,
		viewport.y, viewport.height,
		near, far );
}


void Camera::orthographic( float left, float right, float bottom, float top, float near, float far )
{
	math::Vec3 mid;
	mid.x = ( left + right ) / ( right - left );
	mid.y = ( bottom + top ) / ( bottom - top );
	mid.z = near / ( near - far );

	math::Vec3 scale;
	scale.x = 2.0f / ( right - left );
	scale.y = 2.0f / ( bottom - top );
	scale.z = 1.0f / ( near - far );

	proj = math::Mat4::identity;

	proj.matrix[12] = -mid.x;
	proj.matrix[13] = -mid.y;
	proj.matrix[14] = mid.z;

	proj.matrix[0] = scale.x;
	proj.matrix[5] = scale.y;
	proj.matrix[10] = scale.z;
}


void Camera::perspective( const float a, const float y, const float f, const float n )
{
	assert( f > n && "Far should be greater than near" );

	// Calculate projection matrix
	float cotfov = 1.0f / std::tan( 0.5f * y );
	proj.matrix[0] = cotfov / a;
	proj.matrix[5] = -cotfov;
	proj.matrix[10] = f / ( ( n - f ) * 2.0f );
	proj.matrix[11] = -1.0f;
	proj.matrix[14] = n * f / ( ( n - f ) * 2.0f );
}


void Camera::look_at( const math::Vec3& eye, const math::Vec3& center, math::Vec3 up )
{
	math::Vec3 forward = eye - center;
	forward.normalize();

	math::Vec3 right = math::Vec3::cross( up, forward );
	right.normalize();

	up = math::Vec3::cross( forward, right );
	up.normalize();

	view( 0, 0 ) = right.x;
	view( 0, 1 ) = right.y;
	view( 0, 2 ) = right.z;
	view( 0, 3 ) = -math::Vec3::dot( right, eye );
	view( 1, 0 ) = up.x;
	view( 1, 1 ) = up.y;
	view( 1, 2 ) = up.z;
	view( 1, 3 ) = -math::Vec3::dot( up, eye );
	view( 2, 0 ) = forward.x;
	view( 2, 1 ) = forward.y;
	view( 2, 2 ) = forward.z;
	view( 2, 3 ) = -math::Vec3::dot( forward, eye );
	view( 3, 0 ) = 0.0f;
	view( 3, 1 ) = 0.0f;
	view( 3, 2 ) = 0.0f;
	view( 3, 3 ) = 1.0f;
}


} // namespace spot::gfx
