#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Physics2D;
class Collider2D;
class RenderContext;

enum eSimulationMode
{
	STATIC,
	KINEMATIC,
	DYNAMIC
};

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Update( float deltaSeconds );
	void AddForce( Vec2 const& forceValue );
	float GetMass() const;

	void Destroy();
	void TakeCollider( Collider2D* collider );

	void SetPosition( Vec2 const& position );
	void SetVelocity( Vec2 const& newVelocity );
	void SetSimulationMode( eSimulationMode simulationMode );

	Vec2 GetVelocity();
	Vec2 GetPosition();

	void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor );
	void EnableRigidbody();
	void DisableRigidbody();
	bool IsEnabled();

public:
	Physics2D* m_system;
	Collider2D* m_collider;


protected:
	~Rigidbody2D(); //Destroys the collider

protected:
	bool m_isGarbage = false;
	Vec2 m_worldPosition;
	bool m_isEnabled = true;
	Vec2 m_velocity;
	float m_mass = 1.f;
	Vec2 m_forcePerFrame = Vec2(0.f,0.f);
	eSimulationMode m_simulationMode = DYNAMIC;
};