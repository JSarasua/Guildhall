#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics2D/Manifold2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/PhysicsMaterial.hpp"
#include "Engine/Math/AABB2.hpp"

struct Rgba8;
struct LineSegment2;
class Physics2D;
class RenderContext;




enum eOffScreenDirection
{
	ONSCREEN = -1,
	LEFTOFSCREEN,
	RIGHTOFSCREEN
};

enum eCollider2DType
{
	COLLIDER2D_DISC,
	COLLIDER2D_POLYGON,

	NUM_COLLIDER_TYPE
};

class Collider2D
{
	friend Physics2D;

public:
	Collider2D( float restitution = 1.f );

	virtual void UpdateWorldShape()								= 0;

	virtual Vec2 GetClosestPoint( Vec2 const& position ) const	= 0;
	AABB2 GetBounds() const;
	virtual bool Contains( Vec2 const& position ) const			= 0;
	
	void Move( Vec2 const& translator );
	
	bool Intersects( Collider2D const* other ) const;
	bool GetManifold( Collider2D const* other, Manifold2D* manifold ) const;
	float GetMass();
	eSimulationMode GetSimulationMode() const;
	PhysicsMaterial GetPhysicsMaterial() const;
	float GetBounceWith( Collider2D const* other ) const;

	void SetRestitution( float newRestitution );

	virtual void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness ) = 0;

	virtual bool IsCollidingWithWall( LineSegment2 const& wall ) const	= 0;
	virtual eOffScreenDirection IsOffScreen(AABB2 const& bounds ) const = 0;

protected:
	virtual ~Collider2D();

public:

public:
	eCollider2DType m_type;
	Physics2D* m_system = nullptr;
	Rigidbody2D* m_rigidbody = nullptr;
	PhysicsMaterial m_physicsMaterial;
	AABB2 m_bounds;
protected:
	bool m_isGarbage = false;
};
