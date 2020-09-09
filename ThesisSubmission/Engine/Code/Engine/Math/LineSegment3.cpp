#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/MathUtils.hpp"

LineSegment3::LineSegment3( LineSegment3 const& copyFrom )
{
	startPosition = copyFrom.startPosition;
	endPosition = copyFrom.endPosition;
}

LineSegment3::LineSegment3( Vec3 const& initialStart, Vec3 const& initialEnd )
{
	startPosition = initialStart;
	endPosition = initialEnd;
}

float LineSegment3::GetLength() const
{
	return GetDistance3D(startPosition, endPosition );
}

Vec3 LineSegment3::GetIBasisNormal() const
{
	Vec3 iBasis = endPosition - startPosition;
	iBasis.Normalize();

	return iBasis;
}

Vec3 LineSegment3::GetJBasisNormal() const
{
	Vec3 iBasis = GetIBasisNormal();
	Vec3 jBasis = iBasis.GetRotatedAboutZDegrees( 90.f );
	return jBasis;
}

Vec3 LineSegment3::GetCenter() const
{
	float halfLength = GetLength()*0.5f;
	Vec3 displacementToCenter = GetIBasisNormal()*halfLength;
	Vec3 center = startPosition + displacementToCenter;

	return center;
}
