#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Physics2D/Manifold2D.hpp"

class Collider2D;

struct Collision2D
{
public:
	Collider2D* me;
	Collider2D* them;
	Manifold2D manifold;
	IntVec2 colliderId;
};