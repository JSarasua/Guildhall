#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

int absInt( int initialValue )
{
	return abs( initialValue );
}

float absFloat( float initialValue )
{
	return fabsf( initialValue );
}

float SignFloat( float val )
{
	return ( val >= 0.f ) ? 1.f : -1.f;
}

bool AlmostEqualsFloat( float a, float b, float epsilon /*= 0.01f*/ )
{
	float bMin = b - epsilon;
	float bMax = b + epsilon;

	if( a >= bMin && a <= bMax )
	{
		return true;
	}

	return false;
}

float ConvertDegreesToRadians( float orientationDegrees )
{
	float degreesToRadiansScale =  3.14159265f / 180.f;
	return orientationDegrees * degreesToRadiansScale;
}
float ConvertRadiansToDegrees( float orientationRadians )
{
	float RadiansToDegreesScale = 180.f /3.14159265f;
	return orientationRadians * RadiansToDegreesScale;
}
float CosDegrees( float orientationDegrees )
{
	return cosf( ConvertDegreesToRadians( orientationDegrees ) );
}
float SinDegrees( float orientationDegrees )
{
	return sinf( ConvertDegreesToRadians( orientationDegrees ) );
}

float TanDegrees( float orientationDegrees )
{
	return tanf( ConvertDegreesToRadians( orientationDegrees ) );
}

float Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees( atan2f( y, x ) );
}

float GetDistance2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	return sqrtf( (diffX*diffX) + (diffY*diffY) );
}

float GetDistanceSquared2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	return (diffX*diffX) + (diffY*diffY);
}

int GetTaxicabDistance2D( const IntVec2& vectorA, const IntVec2& vectorB )
{
	IntVec2 displacementVec = vectorA - vectorB;
	return displacementVec.GetTaxicabLength();
}

float GetDistance3D( const Vec3& vectorA, const Vec3& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	float diffZ = vectorB.z - vectorA.z;
	return sqrtf( (diffX*diffX) + (diffY*diffY) + (diffZ*diffZ) );
}

float GetDistanceXY3D( const Vec3& vectorA, const Vec3& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	return sqrtf( (diffX*diffX) + (diffY*diffY) );
}

float GetDistanceSquared3D( const Vec3& vectorA, const Vec3& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	float diffZ = vectorB.z - vectorA.z;
	return (diffX*diffX) + (diffY*diffY) + (diffZ*diffZ);
}

float GetDistanceXYSquared3D( const Vec3& vectorA, const Vec3& vectorB )
{
	float diffX = vectorB.x - vectorA.x;
	float diffY = vectorB.y - vectorA.y;
	return (diffX*diffX) + (diffY*diffY);
}

float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	Vec2 normalizedOntoVector = ontoVector.GetNormalized();
	float projectedLength = sourceVector.x*normalizedOntoVector.x + sourceVector.y*normalizedOntoVector.y;
	return projectedLength;
}

const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	Vec2 projectedVector = ontoVector.GetNormalized();
	projectedVector *= GetProjectedLength2D( sourceVector, ontoVector );
	return projectedVector;
}

float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float vecAOrientationDegrees = vectorA.GetAngleDegrees();
	float vecBOrientationDegrees = vectorB.GetAngleDegrees();

	float angleBetweenVectorsDegrees = vecAOrientationDegrees - vecBOrientationDegrees;
	angleBetweenVectorsDegrees = absFloat( angleBetweenVectorsDegrees );
	return angleBetweenVectorsDegrees;
}

bool DoDiscsOverlap( const Vec2& vectorA, float radiusA, const Vec2& vectorB, float radiusB )
{

	float distanceBetweenVectors = GetDistance2D( vectorA, vectorB );
	if( distanceBetweenVectors <  (radiusA+radiusB) )
	{
		return true;
	}
	return false;
}

bool DoSpheresOverlap( const Vec3& vectorA, float radiusA, const Vec3& vectorB, float radiusB )
{
	float distanceBetweenVectors = GetDistance3D( vectorA, vectorB );
	if( distanceBetweenVectors <  (radiusA+radiusB) )
	{
		return true;
	}
	return false;
}

const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	Vec2 transformedVec;
	transformedVec.x = position.x;
	transformedVec.y = position.y;

	transformedVec.x *= uniformScale;
	transformedVec.y *= uniformScale;

	transformedVec.RotateDegrees( rotationDegrees );
	transformedVec.x += translation.x;
	transformedVec.y += translation.y;
	return transformedVec;
}

const Vec2 TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation )
{
	float newX = position.x*iBasis.x + position.y*jBasis.x;
	float newY = position.x*iBasis.y + position.y*jBasis.y;
	Vec2 transformedVec( newX, newY );
	transformedVec += translation;
	return transformedVec;
}

const Vec3 TransformPosition3DXY( const Vec3& position, float scaleXY, float RotationDegrees, const Vec2& translationXY )
{
	Vec3 transformedVec = position.GetRotatedAboutZDegrees( RotationDegrees );
	transformedVec.z = position.z;

	transformedVec.x *= scaleXY;
	transformedVec.y *= scaleXY;

	transformedVec.x += translationXY.x;
	transformedVec.y += translationXY.y;
	return transformedVec;
}

const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY )
{
	float newX = position.x*iBasisXY.x + position.y*jBasisXY.x;
	float newY = position.x*iBasisXY.y + position.y*jBasisXY.y;
	Vec3 transformedVec( newX, newY, position.z );
	transformedVec += translationXY;
	return transformedVec;
}

float RangeMap( float InputMin, float InputMax, float outputMin, float outputMax, float inputScale )
{
	float displacement = inputScale - InputMin;
	float inputRange = InputMax - InputMin;
	float fraction = displacement/inputRange;
	float outputRange = outputMax - outputMin;
	float outputDisplacement = fraction * outputRange;
	float outputValue = outputDisplacement + outputMin;
	return outputValue;
}

float Interpolate( float rangeMin, float rangeMax, float rangeScale )
{
	float displacement = rangeMax - rangeMin;
	float outputValue = displacement * rangeScale;
	outputValue += rangeMin;
	return outputValue;
}


float Clampf( float rawValue, float inputMin, float inputMax )
{
	if( rawValue < inputMin )
	{
		return inputMin;
	}
	else if( rawValue > inputMax )
	{
		return inputMax;
	}
	return rawValue;


}


int ClampInt( int rawValue, int inputMin, int inputMax )
{
	if( rawValue < inputMin )
	{
		return inputMin;
	}
	else if( rawValue > inputMax )
	{
		return inputMax;
	}
	return rawValue;
}

double ClampDouble( double rawValue, double inputMin, double inputMax )
{
	if( rawValue < inputMin )
	{
		return inputMin;
	}
	else if( rawValue > inputMax )
	{
		return inputMax;
	}
	return rawValue;
}

float ClampZeroToOne( float rawValue )
{
	return Clampf( rawValue, 0.f, 1.f );
}


int RoundDownToInt( float rawValue )
{
	int tempInt = static_cast<int>(floorf( rawValue ));
	return tempInt;
}


const Vec2 GetNearestPointOnDisc2D( const Vec2& point, const Vec2& discPosition, float radius )
{
	Vec2 nearestPoint = Vec2( point.x - discPosition.x, point.y - discPosition.y );
	nearestPoint.ClampLength( radius );
	nearestPoint += discPosition;
	return nearestPoint;
}

const Vec2 GetNearestPointOnAABB2D( const AABB2& aabb2, const Vec2& point )
{
	float nearestX = Clampf( point.x, aabb2.mins.x, aabb2.maxs.x );
	float nearestY = Clampf( point.y, aabb2.mins.y, aabb2.maxs.y );

	return Vec2( nearestX, nearestY );
}

const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 infiniteLineDirectionVector = anotherPointOnLine - somePointOnLine;
	Vec2 somePointToRefPos = refPos - somePointOnLine;

	Vec2 refPosProjectedOntoLine = GetProjectedOnto2D( somePointToRefPos, infiniteLineDirectionVector );
	refPosProjectedOntoLine+= somePointOnLine;

	return refPosProjectedOntoLine;
}

const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end )
{
	return GetNearestPointOnCapsule2D( refPos, start, end, 0.f );
}

const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	Vec2 startToRefPos = refPos - capsuleMidStart;
	Vec2 endToRefPos = refPos - capsuleMidEnd;
	Vec2 startToEnd = capsuleMidEnd - capsuleMidStart;
	Vec2 endToStart = capsuleMidStart- capsuleMidEnd;

	float startToRefPosOntoEndToStartProjectedLength = GetProjectedLength2D( startToRefPos, endToStart );
	float endToRefPosOntoStartToEndProjectedLength = GetProjectedLength2D( endToRefPos, startToEnd );

	Vec2 startToRefPosProjectedOntoStartToEnd = GetProjectedOnto2D( startToRefPos, startToEnd );

	if( startToRefPosOntoEndToStartProjectedLength > 0.f )
	{
		//Do disc check from startPos
		return GetNearestPointOnDisc2D( refPos, capsuleMidStart, capsuleRadius );
	}
	else if( endToRefPosOntoStartToEndProjectedLength > 0.f )
	{
		//Do disc check from endPos
		return GetNearestPointOnDisc2D( refPos, capsuleMidEnd, capsuleRadius );
	}
	else
	{
		Capsule2 capsule = Capsule2( capsuleMidStart, capsuleMidEnd, capsuleRadius );
		Vec2 center = capsule.GetCenter();
		Vec2 fullDimensions = Vec2( capsule.GetBoneLength(), capsule.radius * 2.f );
		Vec2 capsuleIBasis = startToEnd;
		capsuleIBasis.Normalize();
		OBB2 capsuleOBB2 = OBB2( center, fullDimensions, capsuleIBasis );

		return GetNearestPointOnOBB2D( refPos, capsuleOBB2 );
	}

}

const Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box )
{
	return box.GetNearestPoint( refPos );
}

// Vec3 const GetNearestPointOnLineSegment3D( Vec3 const& refPos, Vec3 const& start, Vec3 const& end )
// {
// 
// }

float Max( float a, float b )
{
	return (a>b) ? a : b;
}

float Min( float a, float b )
{
	return (a<b) ? a : b;
}

int PositiveMod( int valueToMod, int modBy )
{
	int moddedValue = 0;
	int modFraction = valueToMod/modBy;

	moddedValue = absInt( valueToMod - modFraction*modBy );
	return moddedValue;
}

bool DoAABBsOverlap2D( const AABB2& aabbA, const AABB2& aabbB ) //Slower than checking each case of no overlap, should be 4 if statements
{
	//X
	float maxOfMinsX = Max( aabbA.mins.x, aabbB.mins.x );
	float minOfMaxsX = Min( aabbA.maxs.x, aabbB.maxs.x );

	//Y
	float maxOfMinsY = Max( aabbA.mins.y, aabbB.mins.y );
	float minOfMaxsY = Min( aabbA.maxs.y, aabbB.maxs.y );

	if( aabbA.IsPointInside( aabbB.mins ) || aabbB.IsPointInside( aabbA.mins ) )
	{
		return true;
	}

	if( maxOfMinsX < minOfMaxsX && maxOfMinsY < minOfMaxsY )
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool DoDiscAndAABBOverlap2D( const Vec2& discPosition, float discRadius, const AABB2& aabb )
{
	Vec2 nearestPoint = GetNearestPointOnAABB2D( aabb, discPosition );
	if( GetDistance2D( discPosition, nearestPoint ) < discRadius )
	{
		return true;
	}

	return false;
}

bool DoDiscAndLineSegmentOverlap2D( Vec2 const& discPosition, float discRadius, LineSegment2 const& line )
{
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D( discPosition, line.startPosition, line.endPosition );
	return IsPointInsideDisc2D( nearestPoint, discPosition, discRadius );
}

bool DoLineSegmentsOverlap2D( LineSegment2 const& lineA, LineSegment2 const& lineB )
{
	Vec2 lineAStartAndEndPositions[2];
	Vec2 lineBStartAndEndPositions[2];

	//obb.GetCornerPositions( OBBCornerPositions );
	lineAStartAndEndPositions[0] = lineA.startPosition;
	lineAStartAndEndPositions[1] = lineA.endPosition;

	lineBStartAndEndPositions[0] = lineB.startPosition;
	lineBStartAndEndPositions[1] = lineB.endPosition;

	float lineAHalfLength = lineA.GetLength()*0.5f;
	FloatRange lineAIBasisRange = FloatRange( -lineAHalfLength, lineAHalfLength );
	FloatRange lineBOnLineAIRange = GetRangeOnProjectedAxis( 2, lineBStartAndEndPositions, lineA.GetCenter(), lineA.GetIBasisNormal() );
	if( !lineAIBasisRange.DoesOverlap( lineBOnLineAIRange ) )
	{
		return false;
	}

	FloatRange lineAJBasisRange = FloatRange( 0.f, 0.f );
	FloatRange LineBOnLineAJRange = GetRangeOnProjectedAxis( 2, lineBStartAndEndPositions, lineA.GetCenter(), lineA.GetJBasisNormal() );
	if( !lineAJBasisRange.DoesOverlap( LineBOnLineAJRange ) )
	{
		return false;
	}

	float lineBHalfLength = lineB.GetLength()*0.5f;
	FloatRange lineBIBasisRange = FloatRange( -lineBHalfLength, lineBHalfLength );
	FloatRange lineAOnLineBIRange = GetRangeOnProjectedAxis( 2, lineAStartAndEndPositions, lineB.GetCenter(), lineB.GetIBasisNormal() );
	if( !lineBIBasisRange.DoesOverlap( lineAOnLineBIRange ) )
	{
		return false;
	}

	FloatRange lineBJBasisRange = FloatRange( 0.f, 0.f );
	FloatRange lineAOnLineBJRange = GetRangeOnProjectedAxis( 2, lineAStartAndEndPositions, lineB.GetCenter(), lineB.GetJBasisNormal() );
	if( !lineBJBasisRange.DoesOverlap( lineAOnLineBJRange ) )
	{
		return false;
	}

	return true;
}

FloatRange GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeToPos, const Vec2& axisNormal )
{
	FloatRange rangeOnProjectedAxis( 0.f, 0.f );

	Vec2 pointToRelativePosFirst = points[0] - relativeToPos;
	float projectedValueFirst = GetProjectedLength2D( pointToRelativePosFirst, axisNormal );
	rangeOnProjectedAxis.minimum = projectedValueFirst;
	rangeOnProjectedAxis.maximum = projectedValueFirst;

	for( int pointIndex = 1; pointIndex < numPoints; pointIndex++ )
	{
		Vec2 pointToRelativePos = points[pointIndex] - relativeToPos;
		float projectedValue = GetProjectedLength2D( pointToRelativePos, axisNormal );
		rangeOnProjectedAxis.minimum = Min( projectedValue, rangeOnProjectedAxis.minimum );
		rangeOnProjectedAxis.maximum = Max( projectedValue, rangeOnProjectedAxis.maximum );
	}

	return rangeOnProjectedAxis;
}

bool DoOBBAndOBBOverlap2D( const OBB2& boxA, const OBB2& boxB )
{
	Vec2 boxACornerPositions[4];
	Vec2 boxBCornerPositions[4];

	boxA.GetCornerPositions( boxACornerPositions );
	boxB.GetCornerPositions( boxBCornerPositions );

	FloatRange boxAOnAIRange;
	boxAOnAIRange.minimum = -boxA.m_halfDimensions.x;
	boxAOnAIRange.maximum = boxA.m_halfDimensions.x;
	FloatRange boxBOnAIRange = GetRangeOnProjectedAxis( 4, boxBCornerPositions, boxA.GetCenter(), boxA.GetIBasisNormal() );
	if( !boxBOnAIRange.DoesOverlap( boxAOnAIRange ) )
	{
		return false;
	}

	FloatRange boxAOnAJRange;
	boxAOnAJRange.minimum = -boxA.m_halfDimensions.y;
	boxAOnAJRange.maximum = boxA.m_halfDimensions.y;
	FloatRange boxBOnAJRange = GetRangeOnProjectedAxis( 4, boxBCornerPositions, boxA.GetCenter(), boxA.GetJBasisNormal() );
	if( !boxBOnAJRange.DoesOverlap( boxAOnAJRange ) )
	{
		return false;
	}

	FloatRange boxBOnBIRange;
	boxBOnBIRange.minimum = -boxB.m_halfDimensions.x;
	boxBOnBIRange.maximum = boxB.m_halfDimensions.x;
	FloatRange boxAOnBIRange = GetRangeOnProjectedAxis( 4, boxACornerPositions, boxB.GetCenter(), boxB.GetIBasisNormal() );
	if( !boxAOnBIRange.DoesOverlap( boxBOnBIRange ) )
	{
		return false;
	}

	FloatRange boxBOnBJRange;
	boxBOnBJRange.minimum = -boxB.m_halfDimensions.y;
	boxBOnBJRange.maximum = boxB.m_halfDimensions.y;
	FloatRange boxAOnBJRange = GetRangeOnProjectedAxis( 4, boxACornerPositions, boxB.GetCenter(), boxB.GetJBasisNormal() );
	if( !boxAOnBJRange.DoesOverlap( boxBOnBJRange ) )
	{
		return false;
	}

	return true;
}

bool DoOBBAndAABBOverlap2D( const OBB2& boxA, const AABB2& boxB )
{
	OBB2 boxBOBB2 = OBB2( boxB, 0.f );
	return DoOBBAndOBBOverlap2D( boxA, boxBOBB2 );
}

bool DoOBBAndLineSegmentOverlap2D( const OBB2& obb, const LineSegment2& line )
{
	Vec2 OBBCornerPositions[4];
	Vec2 lineStartAndEndPositions[2];

	obb.GetCornerPositions( OBBCornerPositions );
	lineStartAndEndPositions[0] = line.startPosition;
	lineStartAndEndPositions[1] = line.endPosition;

	float lineHalfLength = line.GetLength()*0.5f;
	FloatRange lineIBasisRange = FloatRange( -lineHalfLength, lineHalfLength );
	FloatRange obbOnLineIRange = GetRangeOnProjectedAxis( 4, OBBCornerPositions, line.GetCenter(), line.GetIBasisNormal() );
	if( !lineIBasisRange.DoesOverlap( obbOnLineIRange ) )
	{
		return false;
	}

	FloatRange lineJBasisRange = FloatRange( 0.f, 0.f );
	FloatRange obbOnLineJRange = GetRangeOnProjectedAxis( 4, OBBCornerPositions, line.GetCenter(), line.GetJBasisNormal() );
	if( !lineJBasisRange.DoesOverlap( obbOnLineJRange ) )
	{
		return false;
	}

	FloatRange obbOnObbIRange;
	obbOnObbIRange.minimum = -obb.m_halfDimensions.x;
	obbOnObbIRange.maximum = obb.m_halfDimensions.x;
	FloatRange lineOnObbIRange = GetRangeOnProjectedAxis( 2, lineStartAndEndPositions, obb.GetCenter(), obb.GetIBasisNormal() );
	if( !obbOnObbIRange.DoesOverlap( lineOnObbIRange ) )
	{
		return false;
	}

	FloatRange obbOnObbJRange;
	obbOnObbJRange.minimum = -obb.m_halfDimensions.y;
	obbOnObbJRange.maximum = obb.m_halfDimensions.y;
	FloatRange lineOnObbJRange = GetRangeOnProjectedAxis( 2, lineStartAndEndPositions, obb.GetCenter(), obb.GetJBasisNormal() );
	if( !obbOnObbJRange.DoesOverlap( lineOnObbJRange ) )
	{
		return false;
	}

	return true;
}

bool DoOBBAndCapsuleOverlap2D( const OBB2& obb, const Capsule2& capsule )
{
	if( DoOBBAndOBBOverlap2D( obb, capsule.GetInnerBox() ) )
	{
		return true;
	}
	else if( DoOBBAndDiscOverlap2D( obb, capsule.startPosition, capsule.radius ) )
	{
		return true;
	}
	else if( DoOBBAndDiscOverlap2D( obb, capsule.endPosition, capsule.radius ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoOBBAndDiscOverlap2D( const OBB2& obb, const Vec2& discCenter, float discRadius )
{
	Vec2 nearestPointOnOBB2 = obb.GetNearestPoint( discCenter );
	if( GetDistance2D( nearestPointOnOBB2, discCenter ) < discRadius )
	{
		return true;
	}

	return false;
}

bool DoPolygonAndDiscOverlap2D( const Polygon2D& poly, const Vec2& discCenter, float discRadius )
{
	Vec2 polyClosestPoint = poly.GetClosestPoint( discCenter );

	return IsPointInsideDisc2D(polyClosestPoint, discCenter, discRadius );
}

bool DoPolygonAndLineSegementOverlap2D( const Polygon2D& poly, const LineSegment2& line )
{
	size_t polyEdgeCount = poly.GetEdgeCount();

	for( size_t polyEdgeIndex = 0; polyEdgeIndex < polyEdgeCount; polyEdgeIndex++ )
	{
		LineSegment2 edgeLine;
		poly.GetEdge( &edgeLine.startPosition, &edgeLine.endPosition, polyEdgeIndex );
		if( DoLineSegmentsOverlap2D( edgeLine, line ) )
		{
			return true;
		}
	}

	if( poly.Contains( line.startPosition ) )
	{
		return true;
	}

	return false;

	UNUSED( poly );
	UNUSED( line );
}

bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	return DoDiscsOverlap( point, 0.f, discCenter, discRadius );
}

bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box )
{
	return box.IsPointInside( point );
}

bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	if( IsPointInsideDisc2D( point, capsuleMidStart, capsuleRadius ) )
	{
		return true;
	}
	if( IsPointInsideDisc2D( point, capsuleMidEnd, capsuleRadius ) )
	{
		return true;
	}

	Vec2 nearestPointOnCapsule = GetNearestPointOnLineSegment2D( point, capsuleMidStart, capsuleMidEnd );
	float distanceToPoint = GetDistance2D( point, nearestPointOnCapsule );
	if( distanceToPoint < capsuleRadius )
	{
		return true;
	}

	return false;
}

bool IsPointInsideCapsule2D( const Vec2& point, const Capsule2& capsule )
{
	return IsPointInsideCapsule2D(point, capsule.startPosition, capsule.endPosition, capsule.radius );
}

bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box )
{
	return box.IsPointInside( point );
}

bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float observerForwardDegrees, float apertureDegrees, float maxDist )
{
	Vec2 displacementVec = point - observerPos;
	float distanceBetweenPoints = displacementVec.GetLength();

	if( distanceBetweenPoints > maxDist )
	{
		return false;
	}

	float angularDisplacementDegrees = GetShortestAngularDisplacement( observerForwardDegrees, displacementVec.GetAngleDegrees() );
	angularDisplacementDegrees = absFloat( angularDisplacementDegrees );

	if( angularDisplacementDegrees < apertureDegrees*0.5f )
	{
		return true;
	}

	return false;
}

void PushDiscsOutOfEachOther2D( Vec2& discA, float discARadius, Vec2& discB, float discBRadius )
{
	if( !DoDiscsOverlap( discA, discARadius, discB, discBRadius ) )
	{
		return;
	}

	float overlap = (discARadius+discBRadius) - GetDistance2D( discA, discB );
	overlap /= 2.f;
	Vec2 pushDiscs( discA - discB );
	pushDiscs.ClampLength( overlap );
	discA += pushDiscs;
	discB -= pushDiscs;
}

void PushDiscOutOfDisc2D( Vec2& discA, float discARadius, const Vec2& discB, float discBRadius )
{
	if( !DoDiscsOverlap( discA, discARadius, discB, discBRadius ) )
	{
		return;
	}

	float overlap = (discARadius+discBRadius) - GetDistance2D( discA, discB );
	Vec2 pushDiscs( discA - discB );
	pushDiscs.ClampLength( overlap );
	discA += pushDiscs;
}

void PushDiscOutOfPoint2D( Vec2& disc, float discRadius, const Vec2& point )
{
	//Get how far past radius point is.
	//Move disc that far back
	if( !DoDiscsOverlap( disc, discRadius, point, 0.f ) )
	{
		return;
	}
	float overlap = discRadius - GetDistance2D( disc, point );
	Vec2 pushDisc( disc - point );
	pushDisc.ClampLength( overlap );
	disc += pushDisc;
}

void PushDiscOutOfAABB2D( Vec2& disc, float discRadius, const AABB2& aabb )
{
	if( !DoDiscAndAABBOverlap2D( disc, discRadius, aabb ) || GetNearestPointOnAABB2D( aabb, disc ) == disc )
	{
		return;
	}
	Vec2 nearestPoint( GetNearestPointOnAABB2D( aabb, disc ) );
	PushDiscOutOfPoint2D( disc, discRadius, nearestPoint );
}

float GetShortestAngularDisplacement( float oldOrientationDegrees, float newOrientationDegrees )
{

	float displacement = newOrientationDegrees - oldOrientationDegrees;
	while( displacement > 360.f || displacement < -360.f )
	{
		if( displacement > 360.f )
		{
			displacement -= 360.f;
		}
		else
		{
			displacement += 360.f;
		}
	}
	//float negativeDisplacement = oldOrientationDegrees - newOrientationDegrees;
	if( displacement < 180.f && displacement > 0.f )
	{
		return displacement;
	}
	else if( displacement > 180.f )
	{
		return displacement - 360.f;
	}
	else if( displacement > -180.f && displacement < 0.f )
	{
		return displacement;
	}
	else if( displacement < -180.f )
	{
		return 360.f + displacement;
	}
	else
	{
		return 0.f;
	}
}

float GetShortestAngularDistance( float oldOrientationDegrees, float newOrientationDegrees )
{
	float angularDistance = GetShortestAngularDisplacement( oldOrientationDegrees, newOrientationDegrees );
	angularDistance = absFloat( angularDistance );
	return angularDistance;
}

float GetTurnedToward( float oldOrientationDegrees, float newOrientationDegrees, float rotateSpeed )
{
	float displacement = newOrientationDegrees - oldOrientationDegrees;
	while( displacement > 360.f || displacement < -360.f )
	{
		if( displacement > 360.f )
		{
			displacement -= 360.f;
		}
		else
		{
			displacement += 360.f;
		}
	}

	if( absFloat( displacement ) < rotateSpeed || 360.f - absFloat( displacement ) < rotateSpeed )
	{
		return newOrientationDegrees;
	}
	else if( GetShortestAngularDisplacement( oldOrientationDegrees, newOrientationDegrees ) >= 0.f )
	{
		return oldOrientationDegrees + rotateSpeed;
	}
	else
	{
		return oldOrientationDegrees - rotateSpeed;
	}
}

float GetAngleBetweenMinus180And180Degrees( float currentAngleDegrees )
{
	while( currentAngleDegrees > 180.f )
	{
		currentAngleDegrees -= 360.f;
	}
	while( currentAngleDegrees <= -180.f )
	{
		currentAngleDegrees += 360.f;
	}

	return currentAngleDegrees;
}

float DotProduct2D( const Vec2& vecA, const Vec2& vecB )
{
	return vecA.x*vecB.x + vecA.y*vecB.y;
}

Vec3 CrossProduct( Vec3 const& a, Vec3 const& b )
{
	float x = a.y*b.z - a.z*b.y;
	float y = a.z*b.x - a.x*b.z;
	float z = a.x*b.y - a.y*b.x;

	Vec3 c = Vec3( x, y, z );
	return c;
}

float SmoothStart2( float t )
{
	float smoothStart2 = t*t;
	return smoothStart2;
}

float SmoothStart3( float t )
{
	float smoothStart3 = t*t*t;
	return smoothStart3;
}

float SmoothStart4( float t )
{
	float smoothStart4 = t*t*t*t;
	return smoothStart4;
}

float SmoothStart5( float t )
{
	float smoothStart5 = t*t*t*t*t;
	return smoothStart5;
}

float SmoothStop2( float t )
{
	float scale = 1-t;
	float smoothStop2 = 1 - scale*scale;

	return smoothStop2;
}

float SmoothStop3( float t )
{
	float scale = 1-t;
	float smoothStop3 = 1 - scale*scale*scale;

	return smoothStop3;
}

float SmoothStop4( float t )
{
	float scale = 1-t;
	float smoothStop4 = 1 - scale*scale*scale*scale;

	return smoothStop4;
}

float SmoothStop5( float t )
{
	float scale = 1-t;
	float smoothStop5 = 1 - scale*scale*scale*scale*scale;

	return smoothStop5;
}

float SmoothStep3( float t )
{
	float smoothStep3 = 2*(t*t*t) - 3*(t*t);
	return smoothStep3;
}

