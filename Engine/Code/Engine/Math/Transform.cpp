#include "Engine/Math/Transform.hpp"


void Transform::SetPosition( Vec3 const& pos )
{
	m_position = pos;
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	m_rotationPitchRollYawDegrees.x = pitch;
	m_rotationPitchRollYawDegrees.y = roll;
	m_rotationPitchRollYawDegrees.z = yaw;
}

Mat44 Transform::ToMatrix() const
{
	Mat44 mat;
	mat.CreateNonUniformScale3D(m_scale);
	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );
	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
	mat.Translate3D( m_position );

	return mat;

}

