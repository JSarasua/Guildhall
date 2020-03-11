#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

PolygonCollider2D::PolygonCollider2D( Vec2 const& localPosition, Vec2 const& worldPosition, Polygon2D const& polygon, float restitution )
	:m_localPosition(localPosition),
	m_worldPosition(worldPosition),
	m_polygon(polygon),
	Collider2D( restitution )
{
	m_type = COLLIDER2D_POLYGON;

	GUARANTEE_OR_DIE(m_polygon.IsConvex(), "Polygon is not convex");
}

void PolygonCollider2D::UpdateWorldShape()
{
	Vec2 newWorldPosition = m_rigidbody->GetPosition();
	Vec2 currentWorldPosition = m_worldPosition;

	Vec2 translator = newWorldPosition - currentWorldPosition;

	m_polygon.Translate( translator );
	m_worldPosition = m_rigidbody->GetPosition();

	float orientationDegrees = ConvertRadiansToDegrees( m_orientationRadians );
	float newOrientationDegrees = ConvertRadiansToDegrees( m_rigidbody->GetOrientationRadians() );
	float rotationDegrees = GetShortestAngularDisplacement( orientationDegrees, newOrientationDegrees );
	float rotationRadians = ConvertDegreesToRadians( rotationDegrees );
	m_polygon.RotateAroundCenter( rotationRadians );

	m_orientationRadians = m_rigidbody->GetOrientationRadians();

	m_bounds = m_polygon.GetTightlyFixBox();
}

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 const& position ) const
{
	return m_polygon.GetClosestPoint( position );
}

bool PolygonCollider2D::Contains( Vec2 const& position ) const
{
	if( nullptr == m_rigidbody )
	{
		return false;
	}

	return m_polygon.Contains( position );
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

float PolygonCollider2D::CalculateMoment( float mass ) const
{
	//WRONG EQUATION
// 	float vertices = (float)m_polygon.GetVertexCount();
// 	AABB2 aabb2 = GetBounds();
// 	float radius = 0.5f * GetDistance2D( aabb2.mins, aabb2.maxs ) ;
// 
// 	float innerArg = sinf(3.14159265f/vertices);
// 
// 	float moment = 0.5f * mass * (radius * radius) * ( 1.f - 2.f/3.f * (innerArg * innerArg) );
// 	
// 	return moment;


/*	float mass = GetMass();*/
	float area = m_polygon.GetArea();

	float sumOfTriangleMoments = 0.f;

	for( size_t triangleIndex = 0; triangleIndex < m_polygon.GetTriangleCount(); triangleIndex++ )
	{
		sumOfTriangleMoments += CalculateMomentOfATriangle( triangleIndex );
	}

	float moment = (mass/area) * sumOfTriangleMoments;

	return moment;
}

Vec2 PolygonCollider2D::GetCenterOfMass() const
{
	return m_polygon.GetCenterOfMass();
}

void PolygonCollider2D::DebugRender( RenderContext* context, Rgba8 const& borderColor, Rgba8 const& fillColor, float thickness )
{
	context->DrawPolygon2D( m_polygon, fillColor, borderColor, thickness );
}

Polygon2D PolygonCollider2D::GetPolygon() const
{
	return m_polygon;
}

float PolygonCollider2D::CalculateMomentOfATriangle( size_t triangleIndex ) const
{
	//Vertices of the triangle
	Vec2 a;
	Vec2 b;
	Vec2 c;

	m_polygon.GetTriangle( &a, &b, &c, triangleIndex );

	Vec2 centerOfTriangle = m_polygon.GetCenterOfTriangle( triangleIndex );
	Vec2 centerOfMass = GetCenterOfMass();

	Vec2 pivotVector = centerOfTriangle - centerOfMass;
	float pivotDotProduct = DotProduct2D( pivotVector, pivotVector );
	float area = m_polygon.GetAreaOfTriangle( triangleIndex );
	
	
	float parallelAxisPiece = area * pivotDotProduct; //Added to moment



	Vec2 ac = c - a;
	Vec2 ab = b - a;
	Vec2 abPerp = ab.GetRotated90Degrees();

	float base = ab.GetLength();
	float height = GetProjectedLength2D( ac, abPerp );
	height = absFloat( height );

	//old equation
	//float moment = (base*height)/36 * ( (base*base) - (base*area) + (area*area) + (height*height) ); //moment around itself
	
	//Equation off of the internet
	float moment = base * height * height * height /36.f;
	moment += parallelAxisPiece;

	return moment;
}

