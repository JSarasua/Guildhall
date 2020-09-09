#pragma once
#include "Engine/Math/vec2.hpp"

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	~Vec4() {}
	Vec4() {}
	Vec4( const Vec4& copyFrom );
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );
	explicit Vec4( Vec2 const& XY, Vec2 const& ZW );

	const bool	operator==( Vec4 const& b );
	const bool	operator!=( Vec4 const& b );
	const Vec4	operator/( float inverseScale ) const;			// Vec4 / float

	void		operator/=( const float uniformDivisor );

	void	SetFromText(const char* text);

};