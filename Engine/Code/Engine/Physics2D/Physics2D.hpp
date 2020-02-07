#pragma once
#include "Engine/Math/vec2.hpp"
#include <vector>

class Rigidbody2D;
class DiscCollider2D;
class PolygonCollider2D;
class Collider2D;
class Polygon2D;

class Physics2D
{
public:
	void BeginFrame();
	void Update( float deltaSeconds );
	void EndFrame();

	//factory style create/destroy

	Rigidbody2D* CreateRigidBody();
	void DestroyRigidBody( Rigidbody2D* rb );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition, float radius );
	PolygonCollider2D* CreatePolygonCollider( Polygon2D const& poly, Vec2 localPosition );
	void DestroyCollider( Collider2D* collider );

	void SetSceneGravity( float newGravity );
	float GetSceneGravity() const;

private:
	std::vector<Rigidbody2D*> m_rigidBodies;
	std::vector<Collider2D*> m_colliders;

	float m_gravity = 10.f;
};