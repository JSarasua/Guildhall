#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

PolygonCollider2D::PolygonCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, Polygon2D const& polygon )
	:m_localPosition(localPosition),
	m_worldPosition(worldPosition),
	m_polygon(polygon)
{
	m_type = COLLIDER2D_POLYGON;

	GUARANTEE_OR_DIE(m_polygon.IsConvex(), "Polygon is not convex");
}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 translator = m_rigidbody->GetPosition() - m_worldPosition;

	m_polygon.Translate( translator );
	m_worldPosition = m_rigidbody->GetPosition();
}

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 const& position ) const
{
	return m_polygon.GetClosestPoint( position );
}

bool PolygonCollider2D::Contains( Vec2 const& position ) const
{
	if( nullptr != m_rigidbody && !m_rigidbody->IsEnabled() )
	{
		return false;
	}

	return m_polygon.Contains( position );
}

bool PolygonCollider2D::Intersects( Collider2D const* other ) const
{
	if( nullptr != m_rigidbody && !m_rigidbody->IsEnabled() )
	{
		return false;
	}

	switch( other->m_type )
	{
	case COLLIDER2D_DISC:
		//DiscCollider2D const* otherDisc = (DiscCollider2D const*)other;
		return DoPolygonAndDiscOverlap2D( m_polygon, ((DiscCollider2D const*)other)->m_worldPosition, ((DiscCollider2D const*)other)->m_radius );
		break;
	case COLLIDER2D_POLYGON:
		return false;
		break;
	default:
		ERROR_AND_DIE( "Invalid Collider Type" );
		break;
	}
}

bool PolygonCollider2D::IsCollidingWithWall( LineSegment2 const& wall ) const
{
	if( DoPolygonAndLineSegementOverlap2D( m_polygon, wall ) )
	{
		return true;
	}

	return false;
}

eOffScreenDirection PolygonCollider2D::IsOffScreen( AABB2 const& bounds ) const
{
	AABB2 polyBounds = GetBounds();

	if( DoAABBsOverlap2D( polyBounds, bounds ) )
	{
		return ONSCREEN;
	}
	else if( polyBounds.maxs.x < bounds.mins.x )
	{
		return LEFTOFSCREEN;
	}
	else
	{
		return RIGHTOFSCREEN;
	}
}

void PolygonCollider2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness )
{
	context->DrawPolygon2D( m_polygon, fillColor, borderColor, thickness );
}

AABB2 PolygonCollider2D::GetBounds() const
{
	return m_polygon.GetTightlyFixBox();
}

Polygon2D PolygonCollider2D::GetPolygon() const
{
	return m_polygon;
}

