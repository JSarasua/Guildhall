#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/LineSegment2.hpp"


struct Manifold2D
{
public:
	Vec2 normal;
	float penetration;
	//Vec2 contactPoint;
	LineSegment2 contactEdge;
};