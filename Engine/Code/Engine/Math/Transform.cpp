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
	//Clamping may not be necessary
	pitch = Clampf( pitch, -90.f, 90.f );

	pitch = GetAngleBetweenMinus180And180Degrees( pitch );
	roll = GetAngleBetweenMinus180And180Degrees( roll );
	yaw = GetAngleBetweenMinus180And180Degrees( yaw );

	m_rotationPitchRollYawDegrees.x = pitch;
	m_rotationPitchRollYawDegrees.y = roll;
	m_rotationPitchRollYawDegrees.z = yaw;
}

void Transform::SetUniformScale( float scale )
{
	m_scale.x = scale;
	m_scale.y = scale;
	m_scale.z = scale;
}

void Transform::SetNonUniformScale( Vec3 const& scale )
{
	m_scale = scale;
}

Mat44 Transform::ToMatrix() const
{
	Mat44 mat;
	mat.Translate3D( m_position );
	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );
	mat.ScaleNonUniform3D( m_scale );

	return mat;

}

Mat44 Transform::ToRotationMatrix() const
{
	Mat44 mat;
	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );

	return mat;
}

