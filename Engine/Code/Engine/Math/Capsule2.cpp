#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"

Capsule2::Capsule2( const Capsule2& copyFrom )
{
	startPosition	= copyFrom.startPosition;
	endPosition		= copyFrom.endPosition;
	radius			= copyFrom.radius;
}


Capsule2::Capsule2( const Vec2& initialStart, const Vec2& initialEnd, float initialRadius )
{
	startPosition	= initialStart;
	endPosition		= initialEnd;
	radius			= initialRadius;
}

Vec2 Capsule2::GetCenter() const
{
	Vec2 startToEnd = endPosition - startPosition;
	Vec2 centerDisplacement = startToEnd * 0.5f;
	Vec2 center = startPosition + centerDisplacement;

	return center;
}

float Capsule2::GetBoneLength() const
{
	Vec2 startToEnd = endPosition - startPosition;
	float length = startToEnd.GetLength();
	return length;
}

Vec2 Capsule2::GetNearestPoint( const Vec2& point ) const
{
	return GetNearestPointOnCapsule2D(point, startPosition, endPosition, radius);
}

void Capsule2::GetCornerPositions( Vec2* verticePositions ) const
{
	Vec2 startToEnd = endPosition - startPosition;

	float startOrientation = startToEnd.GetAngleDegrees() - 90.f;
	const float increment = 180.f/32.f;

	for( int capsuleIndex = 0; capsuleIndex < 32; capsuleIndex++ )
	{
		float angularDisplacementFromStart = increment * (float)capsuleIndex;
		Vec2 offset = Vec2::MakeFromPolarDegrees(startOrientation + angularDisplacementFromStart, radius);

		verticePositions[capsuleIndex] = endPosition + offset;
	}

	for( int capsuleIndex = 32; capsuleIndex < 64; capsuleIndex++ )
	{
		float angularDisplacementFromStart = increment * (float)capsuleIndex;
		Vec2 offset = Vec2::MakeFromPolarDegrees( startOrientation + angularDisplacementFromStart, radius );

		verticePositions[capsuleIndex] = startPosition + offset;
	}
}

void Capsule2::GetCapsuleBasisVectors( Vec2* capsuleBasisVectors ) const
{
	int numberOfVertices = GetNumberOfVertices();
	Vec2* cornerVertices = new Vec2[numberOfVertices];
	GetCornerPositions(cornerVertices);

	for( int capsuleIndex = 0; capsuleIndex < numberOfVertices; capsuleIndex++ )
	{
		//GetBasis
		if( capsuleIndex == (numberOfVertices-1) )
		{
			capsuleBasisVectors[numberOfVertices] = cornerVertices[capsuleIndex] - cornerVertices[0];
			capsuleBasisVectors[numberOfVertices].Normalize();
		}
		else
		{
			capsuleBasisVectors[numberOfVertices] = cornerVertices[capsuleIndex] - cornerVertices[capsuleIndex+1];
			capsuleBasisVectors[numberOfVertices].Normalize();
		}
	}
}

int Capsule2::GetNumberOfVertices() const
{
	return 64;
}

OBB2 Capsule2::GetTightlyFitOBB2() const
{
	Vec2 center = GetCenter();
	Vec2 dimensions = Vec2(GetBoneLength() + radius*2.f, radius*2.f);
	Vec2 iBasis = GetIBasisNormal();
	OBB2 outerOBB2 = OBB2(center,dimensions,iBasis);

	return outerOBB2;
}

OBB2 Capsule2::GetInnerBox() const
{
	Vec2 center = GetCenter();
	Vec2 dimensions = Vec2( GetBoneLength(), radius*2.f );
	Vec2 iBasis = GetIBasisNormal();
	OBB2 innerOBB2 = OBB2( center, dimensions, iBasis );

	return innerOBB2;
}

Vec2 Capsule2::GetIBasisNormal() const
{
	Vec2 iBasis = endPosition - startPosition;
	iBasis.Normalize();

	return iBasis;
}

bool Capsule2::IsPointInside( const Vec2& point ) const
{
	return IsPointInsideCapsule2D(point, *this);
}

