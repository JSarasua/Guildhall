#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

class Transform
{
public:
	void SetPosition( Vec3 const& pos );
	void Translate( Vec3 const& translator );
	void RotatePitchRollYawDegrees( float pitch, float roll, float yaw );
	void RotatePitchRollYawDegrees( Vec3 const& rotator );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );
	void SetRotationFromPitchRollYawDegrees( Vec3 const& rotator );
	void SetUniformScale( float scale );
	void SetNonUniformScale( Vec3 const& scale );

	Mat44 ToMatrix() const;
	Mat44 ToRotationMatrix() const;

public:
	Vec3 m_position = Vec3( 0.f, 0.f, 0.f );
	Vec3 m_rotationPitchRollYawDegrees = Vec3( 0.f, 0.f, 0.f );
	Vec3 m_scale = Vec3( 1.f, 1.f, 1.f );
};