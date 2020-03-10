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
	void UpdateRotation( float deltaSeconds );
	void AddForce( Vec2 const& forceValue );
	float GetMass() const;
	float GetDrag() const;
	float CalculateMoment();

	void Destroy();
	void TakeCollider( Collider2D* collider );

	void SetPosition( Vec2 const& position );
	void SetMass( float newMass );
	void SetOrientationRadians( float newOrientationRadians );
	void SetAngularVelocityRandians( float newAngularVelocity );
	void Translate( Vec2 const& translator );
	void SetVelocity( Vec2 const& newVelocity );
	void SetSimulationMode( eSimulationMode simulationMode );

	eSimulationMode GetSimulationMode() const;

	void ApplyImpulseAt( Vec2 const& worldPos, Vec2 const& impulse );
	void ApplyDragForce();

	void SetDragCoefficient( float dragCoefficient );


	Vec2 GetVelocity();
	Vec2 GetVerletVelocity();
	Vec2 GetPosition();
	Vec2 GetImpactVelocityAtPoint( Vec2 const& point );

	void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor );
	void EnableRigidbody();
	void DisableRigidbody();
	bool IsEnabled();

	float GetMoment() const;
	float GetOrientationRadians() const;
	float GetAngularVelocityRadians() const;
public:
	Physics2D* m_system;
	Collider2D* m_collider;


protected:
	~Rigidbody2D(); //Destroys the collider

protected:
	bool m_isGarbage = false;
	Vec2 m_worldPosition;
	Vec2 m_frameStartPosition;
	bool m_isEnabled = true;
	Vec2 m_velocity;
	float m_mass = 1.f;
	float m_drag = 0.f;

	Vec2 m_forcePerFrame = Vec2(0.f,0.f);
	eSimulationMode m_simulationMode = DYNAMIC;

	float m_orientationInRadians = 0.f;	//Radians
	float m_angularVelocity = 0.f;		//Radians/Sec
	float m_frameTorque = 0.f;			//???
	float m_moment = 0.f;				//Moment of Inertia
};