#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"

PolygonCollider2D::PolygonCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, Polygon2D const& polygon )
	:m_localPosition(localPosition),
	m_worldPosition(worldPosition),
	m_polygon(polygon)
{

}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 translator = m_rigidbody->GetPosition() - m_worldPosition;


}

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 const& position ) const
{
	return position;
}

bool PolygonCollider2D::Contains( Vec2 const& position ) const
{
	return true;
}

bool PolygonCollider2D::Intersects( Collider2D const* other ) const
{
	return true;
}

void PolygonCollider2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor )
{

}

