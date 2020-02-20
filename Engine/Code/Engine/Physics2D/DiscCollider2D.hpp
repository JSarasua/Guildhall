#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/AABB2.hpp"

class RenderContext;


class DiscCollider2D : public Collider2D
{
public:
	DiscCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, float radius );

	virtual void UpdateWorldShape() override;

	virtual Vec2 GetClosestPoint( Vec2 const& position ) const override;
	virtual AABB2 GetBounds() const override;
	virtual bool Contains( Vec2 const& position ) const	override;
	virtual bool IsCollidingWithWall( LineSegment2 const& wall ) const override;
	virtual eOffScreenDirection IsOffScreen(AABB2 const& bounds ) const override;

	virtual void DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness ) override;

public:
	Vec2 m_localPosition;
	Vec2 m_worldPosition;
	float m_radius;
};