#pragma once
#include "Engine/Math/vec2.hpp"
#include <vector>

class Rigidbody2D;
class DiscCollider2D;
class Collider2D;

class Physics2D
{
public:
	void BeginFrame();
	void Update();
	void EndFrame();

	//factory style create/destroy

	Rigidbody2D* CreateRigidBody();
	void DestroyRigidBody( Rigidbody2D* rb );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition, float center );
	void DestroyCollider( Collider2D* collider );

private:
	std::vector<Rigidbody2D*> m_rigidBodies;
	std::vector<Collider2D*> m_colliders;
};