#pragma once
#include "Engine/Math/vec2.hpp"
#include <vector>
#include "Engine/Physics2D/Collision2D.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Rigidbody2D;
class DiscCollider2D;
class PolygonCollider2D;
class Collider2D;
class Polygon2D;
class Clock;

class Physics2D
{
public:
	void Startup();
	void SetClock( Clock* clock );

	void BeginFrame();
	void Update();
	void AdvanceSimulation( float deltaSeconds );
	void ApplyEffectors();
	void MoveRigidbodies( float deltaSeconds );
	void DetectCollisions();
	void AddCollision( Collision2D collision );
	void ResolveCollisions();
	void ResolveCollision( Collision2D const& collision );
	void CleanupDestroyedObjects();
	void EndFrame();

	float GetMassRatio( Rigidbody2D* me, Rigidbody2D* them ) const;

	float GetFixedDeltaTime() const;
	void SetFixedDeltaTime( float frameTimeSeconds );

	//factory style create/destroy

	Rigidbody2D* CreateRigidBody();
	void DestroyRigidBody( Rigidbody2D* rb );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition, float radius );
	PolygonCollider2D* CreatePolygonCollider( Polygon2D const& poly, Vec2 localPosition );
	void DestroyCollider( Collider2D* collider );

	void SetSceneGravity( float newGravity );
	float GetSceneGravity() const;

	static bool SetPhysicsUpdate( const EventArgs* args );

public:


private:
	std::vector<Rigidbody2D*> m_rigidBodies;
	std::vector<Collider2D*> m_colliders;
	std::vector<Collision2D> m_collisions;

	float m_gravity = 10.f;


	Timer m_fixedTimeTimer;
	Clock* m_clock;
	uint m_nextColliderID = 0;
};