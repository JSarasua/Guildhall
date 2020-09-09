#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MatrixUtils.hpp"

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

void Transform::RotatePitchRollYawDegrees( Vec3 const& rotator )
{
	RotatePitchRollYawDegrees( rotator.x, rotator.y, rotator.z );
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	//Clamping may not be necessary
	//pitch = Clampf( pitch, -90.f, 90.f );

	pitch = GetAngleBetweenMinus180And180Degrees( pitch );
	roll = GetAngleBetweenMinus180And180Degrees( roll );
	yaw = GetAngleBetweenMinus180And180Degrees( yaw );

	m_rotationPitchRollYawDegrees.x = pitch;
	m_rotationPitchRollYawDegrees.y = roll;
	m_rotationPitchRollYawDegrees.z = yaw;
}

void Transform::SetRotationFromPitchRollYawDegrees( Vec3 const& rotator )
{
	SetRotationFromPitchRollYawDegrees( rotator.x, rotator.y, rotator.z );
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
	float yaw = m_rotationPitchRollYawDegrees.z;
	float pitch = m_rotationPitchRollYawDegrees.x;
	float roll = m_rotationPitchRollYawDegrees.y;

	Mat44 lookAtDir;
	if( g_currentBases == eYawPitchRollRotationOrder::YXZ )
	{
		//lookAtDir = LookAt( Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), Vec3( 0.f, 1.f, 0.f ) );
	}
	else if( g_currentBases == eYawPitchRollRotationOrder::ZYX )
	{
		lookAtDir = LookAt( Vec3( 0.f, 0.f, 0.f ), Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 1.f ) );
	}
		//MatrixInvertOrthoNormal( lookAtDir );

	Mat44 mat;
	mat.Translate3D( m_position );
	mat.RotateYawPitchRollDegress( yaw, pitch, roll );
// 	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
// 	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
// 	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );
	mat.ScaleNonUniform3D( m_scale );
	mat.TransformBy( lookAtDir );

	return mat;
}

Mat44 Transform::ToMatrixWithoutWorldBasis() const
{
	float yaw = m_rotationPitchRollYawDegrees.z;
	float pitch = m_rotationPitchRollYawDegrees.x;
	float roll = m_rotationPitchRollYawDegrees.y;

	Mat44 mat;
	mat.Translate3D( m_position );
	mat.RotateYawPitchRollDegress( yaw, pitch, roll );
	// 	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
	// 	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
	// 	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );
	mat.ScaleNonUniform3D( m_scale );

	return mat;
}

Mat44 Transform::ToRotationMatrix() const
{
	float yaw = m_rotationPitchRollYawDegrees.z;
	float pitch = m_rotationPitchRollYawDegrees.x;
	float roll = m_rotationPitchRollYawDegrees.y;

	Mat44 lookAtDir;
	if( g_currentBases == eYawPitchRollRotationOrder::YXZ )
	{
		//lookAtDir = LookAt( Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), Vec3( 0.f, 1.f, 0.f ) );
	}
	else if( g_currentBases == eYawPitchRollRotationOrder::ZYX )
	{
		lookAtDir = LookAt( Vec3( 0.f, 0.f, 0.f ), Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 1.f ) );
	}

	Mat44 mat;
	mat.RotateYawPitchRollDegress( yaw, pitch, roll );
// 	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
// 	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
// 	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );
	mat.TransformBy( lookAtDir );

	return mat;
}

Mat44 Transform::ToRotationMatrixWithoutWorldBasis() const
{
	float yaw = m_rotationPitchRollYawDegrees.z;
	float pitch = m_rotationPitchRollYawDegrees.x;
	float roll = m_rotationPitchRollYawDegrees.y;

	Mat44 mat;
	mat.RotateYawPitchRollDegress( yaw, pitch, roll );
	// 	mat.RotateZDegrees( m_rotationPitchRollYawDegrees.z );
	// 	mat.RotateYDegrees( m_rotationPitchRollYawDegrees.y );
	// 	mat.RotateXDegrees( m_rotationPitchRollYawDegrees.x );


	return mat;
}

