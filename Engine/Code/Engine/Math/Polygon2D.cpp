#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"


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
	Vec2 currentFarthestVertex;
	float currentMaxDistance = 0.f;
	size_t edgeCount = (size_t)GetEdgeCount();
	for( size_t vertexIndex = 0; vertexIndex < edgeCount; vertexIndex++ )
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

