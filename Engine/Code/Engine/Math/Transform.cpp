#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"

void Transform::SetPosition( Vec3 const& pos )
{
	m_position = pos;
}

void Transform::Translate( Vec3 const& translator )
{
	m_position += translator;
}

void Transform::RotatePitchRollYawDegrees( float pitch, float roll, float yaw )
{
	float newPitch = m_rotationPitchRollYawDegrees.x + pitch;
	float newRoll = m_rotationPitchRollYawDegrees.y + roll;
	float newYaw = m_rotationPitchRollYawDegrees.z + yaw;

	SetRotationFromPitchRollYawDegrees( newPitch, newRoll, newYaw );
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	pitch = Clampf( pitch, -90.f, 90.f );
	roll = Clampf( roll, -180.f, 180.f );
	yaw = Clampf( yaw, -180.f, 180.f );

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

