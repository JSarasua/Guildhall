#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

class Transform
{
public:
	void SetPosition( Vec3 const& pos );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );

	Mat44 ToMatrix() const;

public:
	Vec3 m_position = Vec3( 0.f, 0.f, 0.f );
	Vec3 m_rotationPitchRollYawDegrees = Vec3( 0.f, 0.f, 0.f );
	Vec3 m_scale = Vec3( 0.f, 0.f, 0.f );
};