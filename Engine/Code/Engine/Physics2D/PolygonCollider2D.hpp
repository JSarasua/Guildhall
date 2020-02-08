#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/Polygon2D.hpp"

class RenderContext;


class PolygonCollider2D : public Collider2D
{
public:
	PolygonCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, Polygon2D const& polygon );

	virtual void UpdateWorldShape() override;

	virtual Vec2 GetClosestPoint( Vec2 const& position ) const override;
	virtual bool Contains( Vec2 const& position ) const	override;
	virtual bool Intersects( Collider2D const* other ) const override;
	virtual bool IsCollidingWithWall( LineSegment2 const& wall ) const override;
	virtual eOffScreenDirection IsOffScreen(AABB2 const& bounds ) const override;

	virtual void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness ) override;

	virtual AABB2 GetBounds() const override;

public:
	Vec2 m_localPosition;
	Vec2 m_worldPosition;
	Polygon2D m_polygon;
};