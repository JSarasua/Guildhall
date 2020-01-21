#pragma once
#include "Engine/Math/Vec2.hpp"

class Camera
{
public:
	Camera() {}
	~Camera() {}

	void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;


private:
	Vec2 bLeft;
	Vec2 tRight;


public:
	void Translate2D( const Vec2& cameraDisplacement );
};