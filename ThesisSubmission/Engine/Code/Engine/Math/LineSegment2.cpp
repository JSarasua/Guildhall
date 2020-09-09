#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"

LineSegment2::LineSegment2( const LineSegment2& copyFrom )
{
	startPosition = copyFrom.startPosition;
	endPosition = copyFrom.endPosition;
}

LineSegment2::LineSegment2( const Vec2& initialStart, const Vec2& initialEnd )
{
	startPosition = initialStart;
	endPosition = initialEnd;
}

Vec2 LineSegment2::GetNearestPoint( const Vec2& point ) const
{
	return GetNearestPointOnLineSegment2D(point, startPosition, endPosition);
}

float LineSegment2::GetLength() const
{
	return GetDistance2D(startPosition,endPosition);
}

Vec2 LineSegment2::GetIBasisNormal() const
{
	Vec2 iBasis = endPosition - startPosition;
	iBasis.Normalize();

	return iBasis;
}

Vec2 LineSegment2::GetJBasisNormal() const
{
	Vec2 jBasis = GetIBasisNormal();
	jBasis.Rotate90Degrees();
	return jBasis;
}

Vec2 LineSegment2::GetCenter() const
{
	float halfLength = GetLength()*0.5f;
	Vec2 displacementToCenter = GetIBasisNormal()*halfLength;
	Vec2 center = startPosition + displacementToCenter;

	return center;

}

