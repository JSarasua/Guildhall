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

	for( size_t vertexIndex = 0; vertexIndex < m_points.size(); vertexIndex++ )
	{
		center += m_points[vertexIndex];
	}
	center /= (float)m_points.size();
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

AABB2 Polygon2D::GetTightlyFixBox() const
{
	AABB2 box(m_points[0], m_points[0] );

	for( size_t vertexIndex = 0; vertexIndex < m_points.size(); vertexIndex++ )
	{
		box.StretchToIncludePoint( m_points[vertexIndex] );

	}

	return box;
}

Polygon2D Polygon2D::MakeFromLineLoop( Vec2 const* points, unsigned int pointCount )
{
	return Polygon2D( points, pointCount );

}

Polygon2D Polygon2D::MakeConvexFromPointCloud( Vec2 const* points, unsigned int pointCount )
{
	return Polygon2D( points, pointCount );
}

