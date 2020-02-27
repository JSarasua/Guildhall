#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"


class Camera
{
public:
	Camera() {}
	~Camera() {}

	//void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	void SetProjectionOrthographic( float height, float nearZ = -1.f, float farZ = 1.f );
	
	Vec2 GetClientToWorldPosition( Vec2 clientPos ) const;

	float GetAspectRatio() const;

	void SetOutputSize( const Vec2& size );
	void SetPosition( const Vec3& position );

	void Translate2D( const Vec2& cameraDisplacement );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
private:


public:
	Vec2 m_outputSize;
	Vec2 m_position;
};