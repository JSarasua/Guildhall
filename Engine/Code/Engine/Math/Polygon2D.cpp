#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/LineSegment3.hpp"


Polygon2D::Polygon2D( Vec2 const* points, unsigned int pointCount )
{
	for( unsigned int pointsIndex = 0; pointsIndex < pointCount; pointsIndex++ )
	{
		m_points.push_back(points[pointsIndex]);
	}
}

Polygon2D::Polygon2D( std::vector<Vec2> points )
{
	m_points = points;
}

bool Polygon2D::IsValid() const
{
	if( m_points.size() < 3 )
	{
		return false;
	}

	return true;
}

bool Polygon2D::IsConvex() const
{
	if( !IsValid() )
	{
		return false;
	}

	size_t edgeCount = GetEdgeCount();
	for( size_t edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++ )
	{
		Vec2 startPoint;
		Vec2 endPoint;
		Vec2 edgeNormal;
		Vec2 nextEdgePoint;

		if( edgeIndex == edgeCount - 2 )
		{
			nextEdgePoint = m_points[0];
		}
		else if( edgeIndex == edgeCount - 1 )
		{
			nextEdgePoint = m_points[1];
		}
		else 
		{
			nextEdgePoint = m_points[edgeIndex + 2];
		}

		GetEdge( &startPoint, &endPoint, edgeIndex );
		GetEdgeNormal( &edgeNormal, edgeIndex );
		Vec2 startToRefPoint = nextEdgePoint - startPoint;

		float dotProd = DotProduct2D( edgeNormal, startToRefPoint );

		if( dotProd < 0.f )
		{
			return false;
		}
	}

	return true;
}

bool Polygon2D::Contains( Vec2 const& point ) const
{
	if( !IsConvex() || !IsValid() )
	{
		return false;
	}

	size_t edgeCount = GetEdgeCount();
	for( size_t edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++ )
	{
		Vec2 startPoint;
		Vec2 endPoint;
		Vec2 edgeNormal;

		GetEdge( &startPoint, &endPoint, edgeIndex );
		GetEdgeNormal( &edgeNormal, edgeIndex );
		Vec2 startToRefPoint = point - startPoint;

		float dotProd = DotProduct2D( edgeNormal, startToRefPoint );

		if( dotProd < 0.f )
		{
			return false;
		}
	}

	return true;
}

float Polygon2D::GetArea() const
{
	float area = 0.f;
	size_t triangleCount = GetTriangleCount();

	for( size_t triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++ )
	{
		area += GetAreaOfTriangle( triangleIndex );
	}

	return area;
}

float Polygon2D::GetDistance( Vec2 const& point ) const
{
	if( !IsConvex() )
	{
		return false;
	}

	Vec2 closestPoint = GetClosestPoint( point );

	return GetDistance2D( closestPoint, point );
}

Vec2 Polygon2D::GetClosestPoint( Vec2 const& point ) const
{
	Vec2 closestPoint;
	float closestDistance = -1.f;

	size_t edgeCount = GetEdgeCount();
	for( size_t edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++ )
	{
		Vec2 startPoint;
		Vec2 endPoint;

		GetEdge( &startPoint, &endPoint, edgeIndex );
		Vec2 localClosestPoint = GetNearestPointOnLineSegment2D( point, startPoint, endPoint );
		float localClosestDistance = GetDistanceSquared2D( localClosestPoint, point );


		if( (closestDistance < 0.f) || (localClosestDistance < closestDistance) )
		{
			closestPoint = localClosestPoint;
			closestDistance = localClosestDistance;
		}
	}

	return closestPoint;
}

Vec2 Polygon2D::GetCenterOfMass() const
{
	Vec2 center;
	float polygonArea = GetArea();

	for( size_t triangleIndex = 0; triangleIndex < GetTriangleCount(); triangleIndex++ )
	{
		Vec2 weightedCenterOfTriangle = GetCenterOfTriangle( triangleIndex );
		float weightedArea = GetAreaOfTriangle( triangleIndex );
		weightedArea /= polygonArea;
		weightedCenterOfTriangle *= weightedArea;

		center += weightedCenterOfTriangle;
	}

	return center;
}

int Polygon2D::GetVertexCount() const
{
	return (int)m_points.size();
}

int Polygon2D::GetEdgeCount() const
{
	return (int)m_points.size();
}

void Polygon2D::GetEdge( Vec2* outStart, Vec2* outEnd, size_t edgeIndex ) const
{
	size_t numPoints = m_points.size();

	GUARANTEE_OR_DIE( edgeIndex < numPoints, "Edge Index is invalid");

	if( edgeIndex == numPoints - 1 )
	{
		*outStart = m_points[edgeIndex];
		*outEnd = m_points[0];
	}
	else
	{
		*outStart = m_points[edgeIndex];
		*outEnd = m_points[edgeIndex + 1];
	}

}

void Polygon2D::GetEdgeNormal( Vec2* edgeNormal, size_t edgeIndex ) const
{
	Vec2 startPoint;
	Vec2 endPoint;
	GetEdge( &startPoint, &endPoint, edgeIndex );

	Vec2 fwdVec = endPoint - startPoint;
	fwdVec.Rotate90Degrees();
	fwdVec.Normalize();
	*edgeNormal = fwdVec;
}

void Polygon2D::GetEdgeNormalOutward( Vec2* edgeNormal, size_t edgeIndex ) const
{
	GetEdgeNormal( edgeNormal, edgeIndex );
	*edgeNormal *= -1.f;
}

void Polygon2D::GetTriangle( Vec2* outA, Vec2* outB, Vec2* outC, size_t triangleIndex ) const
{
	*outA = m_points[0];
	*outB = m_points[triangleIndex + 1];
	*outC = m_points[triangleIndex + 2];
}

float Polygon2D::GetAreaOfTriangle( size_t triangleIndex ) const
{
	//3 Vertices of the triangle
	Vec2 a;
	Vec2 b;
	Vec2 c;
	GetTriangle( &a, &b, &c, triangleIndex );

	float area = a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y);
	area *= 0.5f;
	area = absFloat( area );

	return area;
}

Vec2 Polygon2D::GetCenterOfTriangle( size_t triangleIndex ) const
{
	//3 Vertices of the triangle
	Vec2 a;
	Vec2 b;
	Vec2 c;
	GetTriangle( &a, &b, &c, triangleIndex );

	Vec2 center;
	center = a + b + c;
	center /= 3.f;

	return center;
}

size_t Polygon2D::GetTriangleCount() const
{
	return m_points.size() - 2;
}

void Polygon2D::Translate( Vec2 const& translator )
{
	for( size_t vertexIndex = 0; vertexIndex < m_points.size(); vertexIndex++ )
	{
		m_points[vertexIndex] += translator;
	}
}

void Polygon2D::RotateAroundCenter( float rotationRadians )
{
	Vec2 center = GetCenterOfMass();

	for( size_t pointIndex = 0; pointIndex < m_points.size(); pointIndex++ )
	{
		m_points[pointIndex] -= center;
		m_points[pointIndex].RotateRadians(rotationRadians);
		m_points[pointIndex] += center;
	}
}

AABB2 Polygon2D::GetTightlyFixBox() const
{
	AABB2 box(m_points[0], m_points[0] );

	for( size_t vertexIndex = 0; vertexIndex < m_points.size(); vertexIndex++ )
	{
		box.StretchToIncludePoint( m_points[vertexIndex] );

	}

	return box;
}

Vec2 Polygon2D::GetFurthestPointInDirection( Vec2 const& direction )  const
{
	Vec2 currentFarthestVertex = m_points[0];
	float currentMaxDistance = GetProjectedLength2D( currentFarthestVertex, direction );
	size_t edgeCount = (size_t)GetEdgeCount();
	for( size_t vertexIndex = 1; vertexIndex < edgeCount; vertexIndex++ )
	{
		Vec2 const& currentVertex = m_points[vertexIndex];
		float currentDistance = GetProjectedLength2D( currentVertex, direction );
		if( currentDistance > currentMaxDistance )
		{
			currentFarthestVertex = currentVertex;
			currentMaxDistance = currentDistance;
		}
	}

	return currentFarthestVertex;
}

Vec2 Polygon2D::GetGJKSupport( Polygon2D const& otherPoly, Vec2 const& dir ) const
{
	Vec2 myGJKSupport = GetFurthestPointInDirection( dir );
	Vec2 otherGJKSupport = otherPoly.GetFurthestPointInDirection( -dir );

	Vec2 GJKMinkowskiDifference = myGJKSupport - otherGJKSupport;

	return GJKMinkowskiDifference;
}

Polygon2D Polygon2D::MakeFromLineLoop( Vec2 const* points, unsigned int pointCount )
{
	return Polygon2D( points, pointCount );

}

Polygon2D Polygon2D::MakeConvexFromPointCloud( Vec2 const* points, unsigned int pointCount )
{
	return Polygon2D( points, pointCount );
}

void Polygon2D::CreateInitialGJKSimplex( Polygon2D const& poly0, Polygon2D const& poly1, Polygon2D* simplex )
{
	Vec2 rightDir = Vec2( 1.f, 0.f );
	Vec2 leftDir = Vec2( -1.f, 0.f );


	Vec2 C = poly0.GetGJKSupport( poly1, rightDir ); //
	Vec2 origin = Vec2( 0.f, 0.f );
	Vec2 CO = C - origin; // CO = -OC

	Vec2 B = poly0.GetGJKSupport( poly1, -CO );
	Vec2 BO = B - origin;
	Vec2 CB = C - B;

	Vec2 normalDir = Vec2( CrossProduct( CB, CrossProduct( CB, CO ) ) );
	normalDir.Normalize();
	Vec2 A = poly0.GetGJKSupport( poly1, normalDir );

	std::vector<Vec2> vertexes;
	vertexes.push_back( A );
	vertexes.push_back( B );
	vertexes.push_back( C );

	Polygon2D GJKTriangle = Polygon2D( vertexes );

	if( !GJKTriangle.IsConvex() )
	{
		vertexes.clear();
		vertexes.push_back( C );
		vertexes.push_back( B );
		vertexes.push_back( A );
		GJKTriangle = Polygon2D( vertexes );
	}
	*simplex = GJKTriangle;

	Vec3 A3D( simplex->m_points[0] );
	Vec3 B3D( simplex->m_points[1] );
	Vec3 C3D( simplex->m_points[2] );

	LineSegment3 AB ( A3D, B3D );
	LineSegment3 BC ( B3D, C3D );
	LineSegment3 CA ( C3D, A3D );

	DebugAddWorldArrow( AB, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
	DebugAddWorldArrow( BC, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
	DebugAddWorldArrow( CA, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );

 	DebugAddWorldPoint( Vec3(), 0.1f, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldPoint( B3D, 1.f, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldPoint( C3D, 1.f, Rgba8::BLUE, 0.f, DEBUG_RENDER_ALWAYS );

	return;
}

bool Polygon2D::EvolveNormalDir( Polygon2D const& poly0, Polygon2D const& poly1, Polygon2D* simplex )
{
	Vec2 origin = Vec2( 0.f, 0.f );
	Vec2 OA = simplex->m_points[0] - origin;
	Vec2 OB = simplex->m_points[1] - origin;
	Vec2 OC = simplex->m_points[2] - origin;

	Vec2 ABEdgeNormal;
	Vec2 BCEdgeNormal;
	Vec2 CAEdgeNormal;
	simplex->GetEdgeNormalOutward( &ABEdgeNormal, 0 );
	simplex->GetEdgeNormalOutward( &BCEdgeNormal, 1 );
	simplex->GetEdgeNormalOutward( &CAEdgeNormal, 2 );

	float ABNormDotOA = DotProduct2D( ABEdgeNormal, OA );
	float BCNormDotOB = DotProduct2D( BCEdgeNormal, OB );
	float CANormDotOC = DotProduct2D( CAEdgeNormal, OC );

	Vec2 B;
	Vec2 C;
	Vec2 normalDir;

	if( ABNormDotOA > 0.f )
	{
		simplex->GetEdge( &B, &C, 0 );
		normalDir = ABEdgeNormal;
	}
	else if( BCNormDotOB > 0.f )
	{
		simplex->GetEdge( &B, &C, 0 );
		normalDir = BCEdgeNormal;
	}
	else if( CANormDotOC > 0.f )
	{
		simplex->GetEdge( &B, &C, 0 );
		normalDir = CAEdgeNormal;
	}
	else
	{
		//Should never be here since Contains should check for this case
		return false;
	}

	//Find A
	Vec2 A = poly0.GetGJKSupport( poly1, normalDir );

	if( A == B || A == C )
	{
		return false;
	}

	//Create new Poly
	std::vector<Vec2> vertexes;
	vertexes.push_back( A );
	vertexes.push_back( B );
	vertexes.push_back( C );

	Polygon2D GJKTriangle = Polygon2D( vertexes );

	if( !GJKTriangle.IsConvex() )
	{
		vertexes.clear();
		vertexes.push_back( C );
		vertexes.push_back( B );
		vertexes.push_back( A );
		GJKTriangle = Polygon2D( vertexes );
	}
	*simplex = GJKTriangle;


	Vec3 A3D( simplex->m_points[0] );
	Vec3 B3D( simplex->m_points[1] );
	Vec3 C3D( simplex->m_points[2] );

	LineSegment3 AB( A3D, B3D );
	LineSegment3 BC( B3D, C3D );
	LineSegment3 CA( C3D, A3D );

	DebugAddWorldArrow( AB, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
	DebugAddWorldArrow( BC, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
	DebugAddWorldArrow( CA, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );

	return true;
}

