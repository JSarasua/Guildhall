#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

OBB2::OBB2( const OBB2& copyFrom ) :
	m_center( copyFrom.m_center),
	m_halfDimensions( copyFrom.m_halfDimensions ),
	m_iBasis( copyFrom.m_iBasis )
{

}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal /*= Vec2(1.f,0.f) */ ) :
	m_center( center ),
	m_halfDimensions( fullDimensions * 0.5f ),
	m_iBasis( iBasisNormal )
{

}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegress ) :
	m_center( center ),
	m_halfDimensions( fullDimensions * 0.5f )
{
	m_iBasis = Vec2::MakeFromPolarDegrees(orientationDegress);
}

OBB2::OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees/*=0.f */ )
{
	m_center = asAxisAlignedBox.GetCenter();
	m_halfDimensions = asAxisAlignedBox.GetDimensions()*0.5f;
	m_iBasis = Vec2::MakeFromPolarDegrees(orientationDegrees);
}

bool OBB2::IsPointInside( const Vec2& point ) const
{
	Vec2 centerToReference = point - m_center;
	Vec2 jBasis = GetJBasisNormal();

	float plocalI = GetProjectedLength2D( centerToReference, m_iBasis );
	float plocalJ = GetProjectedLength2D( centerToReference, jBasis );

	if( (plocalI > -m_halfDimensions.x) &&
		(plocalI < m_halfDimensions.x) &&
		(plocalJ > -m_halfDimensions.y) &&
		(plocalJ < m_halfDimensions.y) )
	{
		return true;
	}
	return false;
}

const Vec2 OBB2::GetCenter() const
{
	return m_center;
}

const Vec2 OBB2::GetDimensions() const
{
	return m_halfDimensions*2.f;
}

const Vec2 OBB2::GetIBasisNormal() const
{
	return m_iBasis;
}

const Vec2 OBB2::GetJBasisNormal() const
{
	Vec2 jBasis(-m_iBasis.y, m_iBasis.x);
	return jBasis;
}

float OBB2::GetOrientationDegrees() const
{
	float orientationDegrees = m_iBasis.GetAngleDegrees();
	return orientationDegrees;
}

const Vec2 OBB2::GetNearestPoint( const Vec2& referencePos ) const
{
	Vec2 centerToReference = referencePos - m_center;
	Vec2 jBasis = GetJBasisNormal();

	float plocalI = GetProjectedLength2D(centerToReference, m_iBasis);
	float plocalJ = GetProjectedLength2D(centerToReference, jBasis); 



	plocalI = Clampf(plocalI, -m_halfDimensions.x, m_halfDimensions.x);
	plocalJ = Clampf(plocalJ, -m_halfDimensions.y, m_halfDimensions.y);


	Vec2 nearestCoordsInWorldSpace = m_center + plocalI*m_iBasis + plocalJ*jBasis;



	return nearestCoordsInWorldSpace;
}

const Vec2 OBB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 jBasis = GetJBasisNormal();


	Vec2 uvIBasis;
	Vec2 uvJBasis;

	uvIBasis.x = RangeMap(0.f,1.f, -m_iBasis.x, m_iBasis.x, uvCoordsZeroToOne.x);
	uvIBasis.y = RangeMap(0.f,1.f, -m_iBasis.y, m_iBasis.y, uvCoordsZeroToOne.x);

	uvJBasis.x = RangeMap(0.f,1.f, -jBasis.x, jBasis.x, uvCoordsZeroToOne.y);
	uvJBasis.y = RangeMap(0.f,1.f, -jBasis.y, jBasis.y, uvCoordsZeroToOne.y);

	Vec2 pointAtUv = uvIBasis * m_halfDimensions + uvJBasis * m_halfDimensions;

	pointAtUv += m_center;

	return pointAtUv;
}

const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 pointRelativeToCenter = point - m_center;
	Vec2 jBasis = GetJBasisNormal();

	float plocalI = GetProjectedLength2D( pointRelativeToCenter, m_iBasis );
	float plocalJ = GetProjectedLength2D( pointRelativeToCenter, jBasis );


	Vec2 uvForPoint;
	uvForPoint.x = RangeMap(-m_halfDimensions.x, m_halfDimensions.x, 0.f, 1.f, plocalI);
	uvForPoint.y = RangeMap(-m_halfDimensions.y, m_halfDimensions.y, 0.f, 1.f, plocalJ);

	return uvForPoint;
}

float OBB2::GetOuterRadius() const
{
	float outerRadius = m_halfDimensions.x + m_halfDimensions.y;
	return outerRadius;
}

float OBB2::GetInnerRadius() const
{
	float innerRadius = Min(m_halfDimensions.x, m_halfDimensions.y);
	return innerRadius;
}

void OBB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	Vec2 iDimension = m_iBasis*m_halfDimensions.x;
	Vec2 jDimension = GetJBasisNormal()*m_halfDimensions.y;

	out_fourPoints[0] = m_center - iDimension - jDimension;
	out_fourPoints[1] = m_center + iDimension - jDimension;
	out_fourPoints[2] = m_center + iDimension + jDimension;
	out_fourPoints[3] = m_center - iDimension + jDimension;
}

void OBB2::Translate( const Vec2& translation )
{
	m_center += translation;
}

void OBB2::SetCenter( const Vec2& newCenter )
{
	m_center = newCenter;
}

void OBB2::SetDimensions( const Vec2& newDimensions )
{
	m_halfDimensions = newDimensions * 0.5f;
}

void OBB2::SetOrientationDegrees( float completelyNewAbsoluteOrientation )
{
	m_iBasis.SetAngleDegrees(completelyNewAbsoluteOrientation);
}

void OBB2::RotateByDegrees( float relativeRotationDegrees )
{
	m_iBasis.RotateDegrees(relativeRotationDegrees);
}

void OBB2::StretchToIncludePoint( const Vec2& point )
{
	if( IsPointInside( point ) )
	{
		return;
	}


	Vec2 centerToPoint = point - m_center;
	float centerToPointProjectedLengthIBasis = GetProjectedLength2D(centerToPoint, m_iBasis);
	float centerToPointProjectedLengthJBasis = GetProjectedLength2D(centerToPoint, GetJBasisNormal());



	Vec2 distanceToIncreaseDimensions;

	if( centerToPointProjectedLengthIBasis > 0.f )
	{
		distanceToIncreaseDimensions.x = centerToPointProjectedLengthIBasis - m_halfDimensions.x;
		distanceToIncreaseDimensions.x *= 0.5f;
	}
	else if( centerToPointProjectedLengthIBasis < 0.f )
	{
		distanceToIncreaseDimensions.x = centerToPointProjectedLengthIBasis + m_halfDimensions.x;
		distanceToIncreaseDimensions.x *= 0.5f;
	}

	if( centerToPointProjectedLengthJBasis > 0.f )
	{
		distanceToIncreaseDimensions.y = centerToPointProjectedLengthJBasis - m_halfDimensions.y;
		distanceToIncreaseDimensions.y *= 0.5f;
	}
	else if( centerToPointProjectedLengthJBasis < 0.f )
	{
		distanceToIncreaseDimensions.y = centerToPointProjectedLengthJBasis + m_halfDimensions.y;
		distanceToIncreaseDimensions.y *= 0.5f;
	}


	//Move the center in the direction of the point we're stretching for
	m_center += distanceToIncreaseDimensions;

	//Dimensions are always increasing, from both sides, so make the value positive
	distanceToIncreaseDimensions.x = absFloat(distanceToIncreaseDimensions.x);
	distanceToIncreaseDimensions.y = absFloat(distanceToIncreaseDimensions.y);

	m_halfDimensions += distanceToIncreaseDimensions;


}

void OBB2::Fix()
{
	m_halfDimensions.x = absFloat(m_halfDimensions.x);
	m_halfDimensions.y = absFloat(m_halfDimensions.y);

	m_iBasis.Normalize();

	if( m_iBasis.x == 0.f && m_iBasis.y == 0.f )
	{
		m_iBasis.x = 1.f;
	}
}

void OBB2::operator=( const OBB2& assignFrom )
{
	m_center = assignFrom.m_center;
	m_halfDimensions = assignFrom.m_halfDimensions;
	m_iBasis = assignFrom.m_iBasis;
}
