#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"

DiscCollider2D::DiscCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, float radius, float restitution ) 
	: m_localPosition(localPosition),
	m_worldPosition(worldPosition),
	m_radius(radius),
	Collider2D( restitution )
{
	m_type = COLLIDER2D_DISC;
}

void DiscCollider2D::UpdateWorldShape()
{
	m_worldPosition = m_rigidbody->GetPosition();

	Vec2 offSet( m_radius, m_radius );
	AABB2 bounds( m_worldPosition - offSet, m_worldPosition + offSet );
	m_bounds = bounds;
}

Vec2 DiscCollider2D::GetClosestPoint( Vec2 const& position ) const
{
	return GetNearestPointOnDisc2D( position, m_worldPosition, m_radius );
}

bool DiscCollider2D::Contains( Vec2 const& position ) const
{
	if( nullptr == m_rigidbody )
	{
		return false;
	}

	return IsPointInsideDisc2D(position, m_worldPosition, m_radius);
}

bool DiscCollider2D::IsCollidingWithWall( LineSegment2 const& wall ) const
{
	if( DoDiscAndLineSegmentOverlap2D( m_worldPosition, m_radius, wall ) )
	{
		return true;
	}

	return false;
}

eOffScreenDirection DiscCollider2D::IsOffScreen( AABB2 const& bounds ) const
{

	FloatRange discIRange( m_worldPosition.x - m_radius, m_worldPosition.x + m_radius );
	FloatRange boundsIRange( bounds.mins.x, bounds.maxs.x );

	if( discIRange.DoesOverlap( boundsIRange ) )
	{
		return ONSCREEN;
	}
	else if ( discIRange.maximum < boundsIRange.minimum )
	{
		return LEFTOFSCREEN;
	}
	else
	{
		return RIGHTOFSCREEN;
	}
}

float DiscCollider2D::CalculateMoment( float mass ) const
{
	float radius = m_radius;
	float moment = (3.f/2.f) * mass * (radius * radius);

	return moment;
}

Vec2 DiscCollider2D::GetCenterOfMass() const
{
	return m_localPosition + m_worldPosition;
}

void DiscCollider2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness )
{
	context->DrawDisc(m_worldPosition, m_radius, fillColor , borderColor, thickness );
	float orientationRadians = m_rigidbody->GetOrientationRadians();

	Vec2 rightPoint = Vec2::MakeFromPolarRadians( orientationRadians, m_radius );
	rightPoint += m_worldPosition + m_localPosition;
	context->DrawLine( m_worldPosition, rightPoint, borderColor, thickness );
}

