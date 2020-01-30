#pragma once
#include "Engine/Math/Vec2.hpp"


struct Rgba8;

enum eCameraClearBitFlag : unsigned int
{
	CLEAR_COLOR_BIT			= (1 << 0 ),
	CLEAR_DEPTH_BIT			= (1 << 1 ),
	CLEAR_STENCIL_BIT		= (1 << 2 ),
};


class Camera
{
public:
	Camera() {}
	~Camera() {}

	void SetOrthoView( const Vec2& bottomLeft, const Vec2&topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.f, unsigned int stencil = 0 );

public:
	Rgba8 m_clearColor;
	unsigned int m_clearMode = 0;

private:
	Vec2 bLeft;
	Vec2 tRight;


public:
	void Translate2D( const Vec2& cameraDisplacement );
};