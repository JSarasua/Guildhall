#pragma once

struct FloatRange;
struct Vec2;
struct IntVec2;
struct Vec3;
struct AABB2;
struct OBB2;
struct Capsule2;
struct LineSegment2;
class Polygon2D;

int			absInt(int initialValue);
float		absFloat(float initialValue);
float		SignFloat( float val );
bool		AlmostEqualsFloat( float a, float b, float epsilon = 0.01f);
float		NaturalLog( float val );

float		SquareRootFloat( float value );

float		Max( float a, float b );
float		Min( float a, float b );
int			MaxInt( int a, int b );
int			MinInt( int a, int b );
int			PositiveMod(int valueToMod, int modBy);

float		ConvertDegreesToRadians(float orientationDegrees);
float		ConvertRadiansToDegrees(float orientationRadians);
float		CosDegrees(float orientationDegrees);
float		SinDegrees(float orientationDegrees);
float		TanDegrees(float orientationDegrees);
float		Atan2Degrees(float y, float x);


float		GetDistance2D(const Vec2& initialVec, const Vec2& secondVec);
float		GetDistanceSquared2D(const Vec2& vectorA, const Vec2& vectorB);
int			GetTaxicabDistance2D( const IntVec2& vectorA, const IntVec2& vectorB);
float		GetDistance3D(const Vec3& vectorA, const Vec3& vectorB);
float		GetDistanceXY3D(const Vec3& vectorA, const Vec3& vectorB);
float		GetDistanceSquared3D(const Vec3& vectorA, const Vec3& VectorB);
float		GetDistanceXYSquared3D(const Vec3& vectorA, const Vec3& VectorB);
float		GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector);
const Vec2	GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );
float		GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB );

bool DoDiscsOverlap(const Vec2& vectorA, float radiusA, const Vec2& vectorB, float radiusB);
bool DoSpheresOverlap(const Vec3& vectorA, float radiusA, const Vec3& VectorB, float radiusB);

const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation);
const Vec2 TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation );
const Vec3 TransformPosition3DXY( const Vec3& position, float scaleXY, float RotationDegrees, const Vec2& translationXY);
const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY );

float RangeMap( float InputMin, float InputMax, float outputMin, float outputMax, float inputScale );
Vec2 RangeMap( float inputMin, float inputMax, Vec2 outputMin, Vec2 outputMax, float inputScale );
float Interpolate(float rangeMin, float rangeMax, float rangeScale);
float Clampf( float rawValue, float inputMin, float inputMax );
int ClampInt( int rawValue, int inputMin, int inputMax );
double ClampDouble( double rawValue, double inputMin, double inputMax );
float ClampZeroToOne(float rawValue);
int RoundDownToInt(float rawValue);

const Vec2 GetNearestPointOnDisc2D(const Vec2& point, const Vec2& discPosition, float radius);
const Vec2 GetNearestPointOnAABB2D(const AABB2& aabb2, const Vec2& point );
const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine );
const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end );
const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
const Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box );

//Vec3 const GetNearestPointOnLineSegment3D( Vec3 const& refPos, Vec3 const& start, Vec3 const& end );

bool DoAABBsOverlap2D(const AABB2& aabbA, const AABB2& aabbB);
bool DoDiscAndAABBOverlap2D(const Vec2& discPosition, float discRadius, const AABB2& aabb);
bool DoDiscAndLineSegmentOverlap2D( Vec2 const& discPosition, float discRadius, LineSegment2 const& line );
bool DoLineSegmentsOverlap2D( LineSegment2 const& lineA, LineSegment2 const& lineB );
bool DoOBBAndOBBOverlap2D( const OBB2& boxA, const OBB2& boxB );
bool DoOBBAndAABBOverlap2D( const OBB2& boxA, const AABB2& boxB );
bool DoOBBAndLineSegmentOverlap2D( const OBB2& obb, const LineSegment2& line );
bool DoOBBAndCapsuleOverlap2D( const OBB2& obb, const Capsule2& capsule );
bool DoOBBAndDiscOverlap2D( const OBB2& obb, const Vec2& discCenter, float discRadius );
bool DoPolygonAndDiscOverlap2D( const Polygon2D& poly, const Vec2& discCenter, float discRadius );
bool DoPolygonAndLineSegementOverlap2D( const Polygon2D& poly, const LineSegment2& line );
FloatRange GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeToPos, const Vec2& axisNormal );

bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius );
bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box);
bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
bool IsPointInsideCapsule2D( const Vec2& point, const Capsule2& capsule );
bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box );
bool IsPointInForwardSector2D(const Vec2& point, const Vec2& observerPos, float observerForwardDegrees, float apertureDegrees, float maxDist );


void PushDiscsOutOfEachOther2D(Vec2& discA, float discARadius, Vec2& discB, float discBRadius);
void PushDiscOutOfDisc2D(Vec2& discA, float discARadius, const Vec2& discB, float discBRadius);
void PushDiscOutOfPoint2D(Vec2& disc, float discRadius, const Vec2& point);
void PushDiscOutOfAABB2D(Vec2& disc, float discRadius, const AABB2& aabb);


float GetShortestAngularDisplacement(float oldOrientationDegrees, float newOrientationDegrees);
float GetShortestAngularDistance(float oldOrientationDegrees, float newOrientationDegrees);
float GetTurnedToward(float oldOrientationDegrees, float newOrientationDegrees, float rotateSpeed);
float GetAngleBetweenMinus180And180Degrees( float currentAngleDegrees );

float DotProduct2D(const Vec2& vecA, const Vec2& vecB);
float DotProduct3D( Vec3 const& vecA, Vec3 const& vecB );
Vec3 CrossProduct( Vec3 const& a, Vec3 const& b );

float SmoothStart2( float t );	// [0, 1] quadratic ease-in
float SmoothStart3( float t );	// [0, 1] cubic ease-in
float SmoothStart4( float t );	// [0, 1] quartic ease-in
float SmoothStart5( float t );	// [0, 1] quintic ease-in
float SmoothStop2( float t );	// [0, 1] quadratic ease-out
float SmoothStop3( float t );	// [0, 1] cubic ease-out
float SmoothStop4( float t );	// [0, 1] quartic ease-out
float SmoothStop5( float t );	// [0, 1] quintic ease-out
float SmoothStep3( float t );	// [0, 1] quadratic ease-in-out
