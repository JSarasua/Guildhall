#pragma once
#include "Engine/Math/vec2.hpp"


struct Manifold2D
{
public:
	Vec2 normal;
	float penetration;
	Vec2 contactPoint;
};