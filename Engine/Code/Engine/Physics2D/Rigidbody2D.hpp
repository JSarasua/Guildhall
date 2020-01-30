#pragma once
#include "Engine/Math/vec2.hpp"

class Physics2D;
class Collider2D;

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Destroy();
	void TakeCollider( Collider2D* collider );

public:
	Physics2D* m_system;
	Collider2D* m_collider;

	Vec2 m_worldPosition;

protected:
	~Rigidbody2D(); //Destroys the collider

protected:
	bool m_isGarbage = false;
};