#pragma once
#include "Engine/Math/vec2.hpp"

struct Plane2
{
public:
	Vec2 fwdNormal;
	float distance = 0.f;
};