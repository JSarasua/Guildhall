#pragma once
#include "Engine/Math/vec2.hpp"

struct Rgba8;
class Physics2D;
class Rigidbody2D;
class RenderContext;


enum Collider2DType
{
	COLLIDER2D_DISC
};

class Collider2D
{
	friend Physics2D;

public:
	virtual void UpdateWorldShape() const						= 0;

	virtual Vec2 GetClosestPoint( Vec2 const& position ) const	= 0;
	virtual bool Contains( Vec2 const& position ) const			= 0;
	virtual bool Intersects( Collider2D const* other ) const	= 0;

	virtual void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor ) = 0;

protected:
	virtual ~Collider2D() = 0;

public:

public:
	Collider2DType m_type;
	Physics2D* m_system;
	Rigidbody2D* m_rigidbody = nullptr;

protected:
	bool m_isGarbage = false;
};