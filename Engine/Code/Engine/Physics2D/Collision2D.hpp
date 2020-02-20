#pragma once

struct Manifold2D;

class Collider2D;

struct Collision2D
{
public:
	Collider2D* me;
	Collision2D* them;
	Manifold2D* manifold;
};