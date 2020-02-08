#pragma once
#include "Engine/Math/vec2.hpp"

struct Rgba8;
struct LineSegment2;
class Physics2D;
class Rigidbody2D;
class RenderContext;
struct AABB2;

enum eOffScreenDirection
{
	ONSCREEN = -1,
	LEFTOFSCREEN,
	RIGHTOFSCREEN
};

enum Collider2DType
{
	COLLIDER2D_DISC
};

class Collider2D
{
	friend Physics2D;

public:
	virtual void UpdateWorldShape()								= 0;

	virtual Vec2 GetClosestPoint( Vec2 const& position ) const	= 0;
	virtual AABB2 GetBounds() const								= 0;
	virtual bool Contains( Vec2 const& position ) const			= 0;
	virtual bool Intersects( Collider2D const* other ) const	= 0;

	virtual void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness ) = 0;

	virtual bool IsCollidingWithWall( LineSegment2 const& wall ) const	= 0;
	virtual eOffScreenDirection IsOffScreen(AABB2 const& bounds ) const = 0;

protected:
	virtual ~Collider2D();

public:

public:
	Collider2DType m_type;
	Physics2D* m_system = nullptr;
	Rigidbody2D* m_rigidbody = nullptr;

protected:
	bool m_isGarbage = false;
};