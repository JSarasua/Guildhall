#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"


class Camera
{
public:
	Camera() {}
	~Camera() {}

	void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	void SetProjectionOrthographic( float height, float nearZ = -1.f, float farZ = 1.f );
	
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetClientToWorldPosition( Vec2 clientPos ) const;

	float GetAspectRatio() const;

	void SetOutputSize( const Vec2& size );
	void SetPosition( const Vec3& position );

private:
	AABB2 m_cameraView;

	Vec2 m_outputSize;
	Vec2 m_position;

public:
	void Translate2D( const Vec2& cameraDisplacement );
};