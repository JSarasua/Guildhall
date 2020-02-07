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
	return m_polygon.Contains( position );
}

bool PolygonCollider2D::Intersects( Collider2D const* other ) const
{
	switch( other->m_type )
	{
	case COLLIDER2D_DISC:
		//DiscCollider2D const* otherDisc = (DiscCollider2D const*)other;
		return DoPolygonAndDiscOverlap2D( m_polygon, ((DiscCollider2D const*)other)->m_worldPosition, ((DiscCollider2D const*)other)->m_radius );
		break;
	default:
		ERROR_AND_DIE( "Invalid Collider Type" );
		break;
	}
}

void PolygonCollider2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness )
{
	context->DrawPolygon2D( m_polygon, fillColor, borderColor, thickness );
}

