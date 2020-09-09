#pragma once
#include "Engine/Math/Vec3.hpp"

struct LineSegment3
{
public:
	Vec3 startPosition	= Vec3( 0.f, 0.f, 0.f );
	Vec3 endPosition	= Vec3( 0.f, 0.f, 0.f );

public:
	//Construction/Destruction
	~LineSegment3() {}
	LineSegment3() {}
	LineSegment3( LineSegment3 const& copyFrom );
	LineSegment3( Vec3 const& initialStart, Vec3 const& initialEnd );

public:
	float GetLength() const;
	Vec3 GetIBasisNormal() const;
	Vec3 GetJBasisNormal() const;
	Vec3 GetCenter() const;
};