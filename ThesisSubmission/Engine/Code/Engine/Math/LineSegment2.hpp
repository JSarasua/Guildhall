#pragma once
#include "Engine/Math/vec2.hpp"

struct LineSegment2
{
public:
	Vec2 startPosition	= Vec2(0.f,0.f);
	Vec2 endPosition	= Vec2(0.f,0.f);

public:
	//Construction/Destruction
	~LineSegment2() {}
	LineSegment2() {}
	LineSegment2( const LineSegment2& copyFrom );
	LineSegment2( const Vec2& initialStart, const Vec2& initialEnd );


private:
	

public:
	Vec2 GetNearestPoint( const Vec2& point ) const;
	float GetLength() const;
	Vec2 GetIBasisNormal() const;
	Vec2 GetJBasisNormal() const;
	Vec2 GetCenter() const;
};