//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A3.cpp
//
#include "Game/UnitTests_MP1A3.hpp"
#include <stdio.h>
#include <stdlib.h>


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		DO NOT MODIFY ANY CODE BELOW HERE WITHOUT EXPRESS PERMISSION FROM YOUR PROFESSOR
//		(as doing so will be considered cheating and have serious academic consequences)
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Forward declarations for selective helper functions needed here
//
bool IsMostlyEqual( float a, float b, float epsilon=0.001f );
bool IsMostlyEqual( const Vector2Class& vec2, float x, float y );
bool IsMostlyEqual( const Vector2Class& vec2a, const Vector2Class& vec2b );
bool IsMostlyEqual( int a, int b, int epsilon=0 );
bool IsMostlyEqual( const Vector3Class& vec3, float x, float y, float z );
bool IsMostlyEqual( const Vector3Class& vec3a, const Vector3Class& vec3b );


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const AABB2Class& box1, const AABB2Class& box2 )
{
	return	IsMostlyEqual( box1.AABB2_Mins, box2.AABB2_Mins ) &&
			IsMostlyEqual( box1.AABB2_Maxs, box2.AABB2_Maxs );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const AABB2Class& box, const Vector2Class& mins, const Vector2Class& maxs )
{
	return	IsMostlyEqual( box.AABB2_Mins, mins ) &&
			IsMostlyEqual( box.AABB2_Maxs, maxs );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const AABB2Class& box, float minX, float minY, float maxX, float maxY )
{
	return	IsMostlyEqual( box.AABB2_Mins, minX, minY ) &&
			IsMostlyEqual( box.AABB2_Maxs, maxX, maxY );
}


//-----------------------------------------------------------------------------------------------
bool IsEqual( const IntVec2Class& a, const IntVec2Class& b )
{
	return (a.x == b.x) && (a.y == b.y);
}


//-----------------------------------------------------------------------------------------------
bool IsEqual( const IntVec2Class& ivec2, int x, int y )
{
	return (ivec2.x == x) && (ivec2.y == y);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Tests follow
//////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_AABB2Basics()
{
#if defined( ENABLE_TestSet_MP1A3_AABB2Basics )

	AABB2Class box1;
	AABB2Class box2( 1.2f, 3.4f, 5.6f, 7.8f );
	AABB2Class box3( Vector2Class( 2.3f, 4.5f ), Vector2Class( 6.7f, 8.9f ) );
	AABB2Class box4( box2 );
	AABB2Class box5;
	box5 = box3;
	const AABB2Class box6( box5 ); // construct const (read-only) object
	const AABB2Class box7( box6 ); // copy-construct should not try to modify read-only source object

	VerifyTestResult( sizeof( AABB2Class ) == 16, "sizeof(AABB2) was not 16 bytes" );
	VerifyTestResult( IsMostlyEqual( box2, 1.2f, 3.4f, 5.6f, 7.8f ), "AABB2 explicit constructor from (minX, minY, maxX, maxY) failed" );
	VerifyTestResult( IsMostlyEqual( box3, 2.3f, 4.5f, 6.7f, 8.9f ), "AABB2 explicit constructor from (mins, maxs) failed" );
	VerifyTestResult( IsMostlyEqual( box4, 1.2f, 3.4f, 5.6f, 7.8f ), "AABB2 copy constructor failed" );
	VerifyTestResult( IsMostlyEqual( box5, 2.3f, 4.5f, 6.7f, 8.9f ), "AABB2 assignment operator= failed" );

#endif
	return 5; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_AABB2Methods()
{
#if defined( ENABLE_TestSet_MP1A3_AABB2Methods )

	AABB2Class box1( Vector2Class( 3.2f, 2.1f ), Vector2Class( 8.9f, 6.7f ) );
	AABB2Class box2( -4.5f, -3.4f, 2.3f, 1.6f );
	AABB2Class box3( box1 );
	const AABB2Class box4( box2 ); // construct const object
	const AABB2Class box5( box4 ); // copy constructor should not try to modify read-only original
	Vector2Class box1Center( 6.05f, 4.4f );
	const Vector2Class box5Center( -1.1f, -0.9f );
	Vector2Class box1Dimensions( 5.7f, 4.6f );
	const Vector2Class box5Dimensions( 6.8f, 5.0f );

	// Test point inside box
	VerifyTestResult( box1.AABB2_IsPointInside( Vector2Class( 4.3f, 5.4f ) ), "AABB2::IsPointInside() incorrect for point inside box (should be true!)" );

	// Test points outside to east/north/west/south
	VerifyTestResult( !box1.AABB2_IsPointInside( Vector2Class( 9.1f, 5.4f ) ), "AABB2::IsPointInside() incorrect for point east (+x) outside box (should be false!)" );
	VerifyTestResult( !box1.AABB2_IsPointInside( Vector2Class( 4.7f, 7.3f ) ), "AABB2::IsPointInside() incorrect for point north (+y) outside box (should be false!)" );
	VerifyTestResult( !box1.AABB2_IsPointInside( Vector2Class( 1.2f, 4.5f ) ), "AABB2::IsPointInside() incorrect for point west (-x) outside box (should be false!)" );
	VerifyTestResult( !box1.AABB2_IsPointInside( Vector2Class( 4.3f, 1.1f ) ), "AABB2::IsPointInside() incorrect for point south (-y) outside box (should be false!)" );

	// Test points outside to NE/NW/SW/SE
	VerifyTestResult( !box5.AABB2_IsPointInside( Vector2Class( 99.9f, 99.9f ) ), "AABB2::IsPointInside() incorrect for point NE (+x,+y) outside box (should be false!)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	VerifyTestResult( !box5.AABB2_IsPointInside( Vector2Class( -99.9f, 99.9f ) ), "AABB2::IsPointInside() incorrect for point NW (-x,+y) outside box (should be false!)" );
	VerifyTestResult( !box5.AABB2_IsPointInside( Vector2Class( -99.9f, -99.9f ) ), "AABB2::IsPointInside() incorrect for point SW (-x,-y) outside box (should be false!)" );
	VerifyTestResult( !box5.AABB2_IsPointInside( Vector2Class( 99.9f, -99.9f ) ), "AABB2::IsPointInside() incorrect for point SE (+x,-y) outside box (should be false!)" );

	// GetCenter, GetDimensions
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetCenter(), box1Center ), "AABB2::GetCenter() was incorrect" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetCenter(), box5Center ), "AABB2::GetCenter() was incorrect" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetDimensions(), box1Dimensions ), "AABB2::GetDimensions() was incorrect" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetDimensions(), box5Dimensions ), "AABB2::GetDimensions() was incorrect" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	// GetNearestPoint
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetNearestPoint( Vector2Class( 9.7f, 5.1f ) ), 8.9f, 5.1f ), "AABB2::GetNearestPoint() incorrect for point east (+x) outside box" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetNearestPoint( Vector2Class( 6.1f, 9.1f ) ), 6.1f, 6.7f ), "AABB2::GetNearestPoint() incorrect for point north (+y) outside box" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetNearestPoint( Vector2Class( 1.2f, 5.1f ) ), 3.2f, 5.1f ), "AABB2::GetNearestPoint() incorrect for point west (-x) outside box" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetNearestPoint( Vector2Class( 6.1f, 1.3f ) ), 6.1f, 2.1f ), "AABB2::GetNearestPoint() incorrect for point south (-y) outside box" );

	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetNearestPoint( Vector2Class( 99.9f, -0.9f ) ), 2.3f, -0.9f ), "AABB2::GetNearestPoint() incorrect for point east (+x) outside box" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetNearestPoint( Vector2Class( -1.7f, 99.9f ) ), -1.7f, 1.6f ), "AABB2::GetNearestPoint() incorrect for point north (+y) outside box" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetNearestPoint( Vector2Class( -99.9f, -0.9f ) ), -4.5f, -0.9f ), "AABB2::GetNearestPoint() incorrect for point west (-x) outside box" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetNearestPoint( Vector2Class( -1.7f, -99.9f ) ), -1.7f, -3.4f ), "AABB2::GetNearestPoint() incorrect for point south (-y) outside box" );

	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetNearestPoint( Vector2Class( 6.1f, 5.1f ) ), 6.1f, 5.1f ), "AABB2::GetNearestPoint() incorrect for point inside box" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetNearestPoint( Vector2Class( -1.7f, -0.9f ) ), -1.7f, -0.9f ), "AABB2::GetNearestPoint() incorrect for point inside box" );

	// GetPointAtUV
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 0.f, 0.f ) ), box1.AABB2_Mins ), "AABB2::GetPointAtUV() incorrect for uv=0,0 (mins)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 1.f, 1.f ) ), box1.AABB2_Maxs ), "AABB2::GetPointAtUV() incorrect for uv=1,1 (maxs)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 1.f, 0.f ) ), box1.AABB2_Maxs.x, box1.AABB2_Mins.y ), "AABB2::GetPointAtUV() incorrect for uv=1,0 (right bottom)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 0.f, 1.f ) ), box1.AABB2_Mins.x, box1.AABB2_Maxs.y ), "AABB2::GetPointAtUV() incorrect for uv=0,1 (left top)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 0.5f, 0.5f ) ), box1Center ), "AABB2::GetPointAtUV() incorrect for uv=.5,.5 (center)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetPointAtUV( Vector2Class( 0.2f, 0.7f ) ), Vector2Class( 4.34f, 5.32f ) ), "AABB2::GetPointAtUV() incorrect for uv=(.2,.7)" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetPointAtUV( Vector2Class( -0.3f, 2.7f ) ), Vector2Class( -6.54f, 10.1f ) ), "AABB2::GetPointAtUV() incorrect for uv=(-.3,2.7)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	// GetUVForPoint
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetUVForPoint( box1.AABB2_Mins ), Vector2Class( 0.f, 0.f ) ), "AABB2::GetUVForPoint() incorrect for mins (uv=0,0)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetUVForPoint( box1.AABB2_Maxs ), Vector2Class( 1.f, 1.f ) ), "AABB2::GetUVForPoint() incorrect for maxs (uv=1,1)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetUVForPoint( box1Center ), Vector2Class( 0.5f, 0.5f ) ), "AABB2::GetUVForPoint() incorrect for center (uv=.5,.5)" );
	VerifyTestResult( IsMostlyEqual( box1.AABB2_GetUVForPoint( Vector2Class( 4.34f, 5.32f ) ), Vector2Class( 0.2f, 0.7f ) ), "AABB2::GetUVForPoint() incorrect for uv=(.2,.7)" );
	VerifyTestResult( IsMostlyEqual( box5.AABB2_GetUVForPoint( Vector2Class( -6.54f, 10.1f ) ), Vector2Class( -0.3f, 2.7f ) ), "AABB2::GetUVForPoint() incorrect for uv=(-.3,2.7)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	// Translate
	box1.AABB2_Translate( Vector2Class( 123.4f, 987.6f ) );
	box2.AABB2_Translate( Vector2Class( -345.6f, -456.7f ) );
	VerifyTestResult( IsMostlyEqual( box1, 126.6f, 989.7f, 132.3f, 994.3f ), "AABB2::Translate() incorrect for (+123.4, +987.6)" );
	VerifyTestResult( IsMostlyEqual( box2, -350.1f, -460.1f, -343.3f, -455.1f ), "AABB2::Translate() incorrect for (-345.6, -456.7)" );

	// SetCenter
	Vector2Class newCenter1( 300.1f, 200.2f );
	Vector2Class newCenter2( -400.3f, -300.4f );
	box1.AABB2_SetCenter( newCenter1 );
	box2.AABB2_SetCenter( newCenter2 );

	Vector2Class actualCenter1 = (box1.AABB2_Mins + box1.AABB2_Maxs) * 0.5f;
	Vector2Class actualCenter2 = (box2.AABB2_Mins + box2.AABB2_Maxs) * 0.5f;
	Vector2Class actualDimensions1 = (box1.AABB2_Maxs - box1.AABB2_Mins);
	Vector2Class actualDimensions2 = (box2.AABB2_Maxs - box2.AABB2_Mins);

	VerifyTestResult( IsMostlyEqual( actualDimensions1, box1Dimensions ), "AABB2::SetCenter() - box dimensions changed (should stay the same!)" );
	VerifyTestResult( IsMostlyEqual( actualDimensions2, box5Dimensions ), "AABB2::SetCenter() - box dimensions changed (should stay the same!)" );
	VerifyTestResult( IsMostlyEqual( actualCenter1, newCenter1 ), "AABB2::SetCenter() - new center was not correct" );
	VerifyTestResult( IsMostlyEqual( actualCenter2, newCenter2 ), "AABB2::SetCenter() - new center was not correct" );
	VerifyTestResult( IsMostlyEqual( box1, 297.25f, 197.9f, 302.95f, 202.5f ), "AABB2::SetCenter() incorrect for (300.1, 200.2)" );
	VerifyTestResult( IsMostlyEqual( box2, -403.7f, -302.9f, -396.9f, -297.9f ), "AABB2::SetCenter() incorrect for (-400.3, -300.4)" );

	// SetDimensions
	Vector2Class startCenter1 = (box1.AABB2_Mins + box1.AABB2_Maxs) * 0.5f;
	Vector2Class startCenter2 = (box2.AABB2_Mins + box2.AABB2_Maxs) * 0.5f;
	Vector2Class startDimensions1 = (box1.AABB2_Maxs - box1.AABB2_Mins);
	Vector2Class startDimensions2 = (box2.AABB2_Maxs - box2.AABB2_Mins);
	Vector2Class newDimensions1( 200.2f, 100.4f );
	Vector2Class newDimensions2( 900.2f, 800.4f );
	box1.AABB2_SetDimensions( newDimensions1 );
	box2.AABB2_SetDimensions( newDimensions2 );

	Vector2Class endCenter1 = (box1.AABB2_Mins + box1.AABB2_Maxs) * 0.5f;
	Vector2Class endCenter2 = (box2.AABB2_Mins + box2.AABB2_Maxs) * 0.5f;
	Vector2Class endDimensions1 = (box1.AABB2_Maxs - box1.AABB2_Mins);
	Vector2Class endDimensions2 = (box2.AABB2_Maxs - box2.AABB2_Mins);

	VerifyTestResult( IsMostlyEqual( startCenter1, endCenter1 ), "AABB2::SetDimensions() - box center changed (should stay the same!)" );
	VerifyTestResult( IsMostlyEqual( startCenter2, endCenter2 ), "AABB2::SetDimensions() - box center changed (should stay the same!)" );
	VerifyTestResult( IsMostlyEqual( endDimensions1, newDimensions1 ), "AABB2::SetDimensions() incorrect for (200.2, 100.4)" );
	VerifyTestResult( IsMostlyEqual( endDimensions2, newDimensions2 ), "AABB2::SetDimensions() incorrect for (900.2, 800.4)" );

	// StretchToIncludePoint
	AABB2Class stretchBox( -2.f, -1.f, 4.f, 3.f );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 0.f, 0.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -2.f, -1.f, 4.f, 3.f ), "AABB2::StretchToIncludePoint() - box should not change if point is inside!" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 99.f, 0.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -2.f, -1.f, 99.f, 3.f ), "AABB2::StretchToIncludePoint() incorrect for point east (+x) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( -99.f, 0.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -99.f, -1.f, 99.f, 3.f ), "AABB2::StretchToIncludePoint() incorrect for point west (-x) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 0.f, 99.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -99.f, -1.f, 99.f, 99.f ), "AABB2::StretchToIncludePoint() incorrect for point north (+y) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 0.f, -99.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -99.f, -99.f, 99.f, 99.f ), "AABB2::StretchToIncludePoint() incorrect for point south (-y) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 222.f, 333.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -99.f, -99.f, 222.f, 333.f ), "AABB2::StretchToIncludePoint() incorrect for point NE (+x,+y) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( -444.f, -555.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -444.f, -555.f, 222.f, 333.f ), "AABB2::StretchToIncludePoint() incorrect for point SW (-x,-y) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( 666.f, -777.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -444.f, -777.f, 666.f, 333.f ), "AABB2::StretchToIncludePoint() incorrect for point SE (+x,-y) outside box" );
	stretchBox.AABB2_StretchToIncludePoint( Vector2Class( -888.f, 999.f ) );
	VerifyTestResult( IsMostlyEqual( stretchBox, -888.f, -777.f, 666.f, 999.f ), "AABB2::StretchToIncludePoint() incorrect for point NW (-x,+y) outside box" );

#endif
	return 56; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_IntVec2Basics()
{
#if defined( ENABLE_TestSet_MP1A3_IntVec2Basics )

	IntVec2Class ivec2a;
	IntVec2Class ivec2b( 4, 3 );
	IntVec2Class ivec2c( -2123456789, -1987654321 ); // should be able to hold any signed integer value in [-2147483648,2147483647]
	IntVec2Class ivec2d( ivec2b );
	IntVec2Class ivec2e;
	ivec2e = ivec2c;
	const IntVec2Class ivec2f( ivec2b ); // construct const object
	const IntVec2Class ivec2g( ivec2f ); // copy construction should take original as:  const IntVec2&

	VerifyTestResult( sizeof( IntVec2Class ) == 8, "sizeof(IntVec2) was not 8 bytes" );
	VerifyTestResult( IsEqual( ivec2b, 4, 3 ), "IntVec2(4,3) was constructed incorrectly" );
	VerifyTestResult( IsEqual( ivec2c, -2123456789, -1987654321 ), "IntVec2( -2123456789, -1987654321 ) constructed incorrectly (are you using ints?)" );
	VerifyTestResult( IsEqual( ivec2d, 4, 3 ), "IntVec2 copy constructor was incorrect" );
	VerifyTestResult( IsEqual( ivec2e, -2123456789, -1987654321 ), "IntVec2 assignment operator= was incorrect" );

#endif
	return 5; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_IntVec2Methods()
{
#if defined( ENABLE_TestSet_MP1A3_IntVec2Methods )

	// GetLength (returns a float), GetLengthSquared (returns an int)
	IntVec2Class ivec2a( 3, 2 );
	IntVec2Class ivec2b( -5, 4 );
	const IntVec2Class ivec2c( 1, -2 ); // All "Get" methods should be "const" so they still work on read-only objects!

	VerifyTestResult( ivec2a.IntVec2_GetLengthSquared() == 13, "IntVec2::GetLengthSquared() incorrect for (3,2)" );
	VerifyTestResult( ivec2b.IntVec2_GetLengthSquared() == 41, "IntVec2::GetLengthSquared() incorrect for (-5,4)" );
	VerifyTestResult( ivec2c.IntVec2_GetLengthSquared() == 5, "IntVec2::GetLengthSquared() incorrect for (1,-2)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	VerifyTestResult( IsMostlyEqual( ivec2a.IntVec2_GetLength(), 3.6055513f ), "IntVec2::GetLength() incorrect for (3,2)" );
	VerifyTestResult( IsMostlyEqual( ivec2b.IntVec2_GetLength(), 6.4031242f ), "IntVec2::GetLength() incorrect for (-5,4)" );
	VerifyTestResult( IsMostlyEqual( ivec2c.IntVec2_GetLength(), 2.2360679f ), "IntVec2::GetLength() incorrect for (1,-2)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	// GetTaxicabLength
	VerifyTestResult( ivec2a.IntVec2_GetTaxicabLength() == 5, "IntVec2::GetTaxicabLength() incorrect for (3,2)" );
	VerifyTestResult( ivec2b.IntVec2_GetTaxicabLength() == 9, "IntVec2::GetTaxicabLength() incorrect for (-5,4)" );
	VerifyTestResult( ivec2c.IntVec2_GetTaxicabLength() == 3, "IntVec2::GetTaxicabLength() incorrect for (1,-2)" ); // NOTE: If this line does not compile, check if you forgot to make this method "const"!

	// GetOrientationRadians, GetOrientationDegrees
	IntVec2Class ivec2e( 7, 0 );
	IntVec2Class ivec2f( 0, 6 );

	float ivec2cDegrees = ivec2c.IntVec2_GetOrientationDegrees(); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	bool is4thQuadrantCorrect = IsMostlyEqual( ivec2cDegrees, -63.434949f )|| IsMostlyEqual( ivec2cDegrees, 296.565051f );
	VerifyTestResult( IsMostlyEqual( ivec2a.IntVec2_GetOrientationRadians(), 0.588003f ),	"IntVec2::GetOrientationRadians() incorrect for (3,2)" );
	VerifyTestResult( IsMostlyEqual( ivec2a.IntVec2_GetOrientationDegrees(), 33.69007f ),	"IntVec2::GetOrientationDegrees() incorrect for (3,2)" );
	VerifyTestResult( IsMostlyEqual( ivec2b.IntVec2_GetOrientationRadians(), 2.466852f ),	"IntVec2::GetOrientationRadians() incorrect for (-5,4)" );
	VerifyTestResult( is4thQuadrantCorrect,													"IntVec2::GetOrientationDegrees() incorrect for (1,-2)" );
	VerifyTestResult( IsMostlyEqual( ivec2e.IntVec2_GetOrientationRadians(), 0.f ),			"IntVec2::GetOrientationRadians() incorrect for (7,0)" );
	VerifyTestResult( IsMostlyEqual( ivec2f.IntVec2_GetOrientationDegrees(), 90.f ),		"IntVec2::GetOrientationDegrees() incorrect for (0,6)" );

	// Rotate 90 degrees, etc.
	IntVec2Class ivec2g = ivec2a.IntVec2_GetRotated90Degrees();
	IntVec2Class ivec2h = ivec2b.IntVec2_GetRotated90Degrees();
	IntVec2Class ivec2i = ivec2c.IntVec2_GetRotated90Degrees(); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	IntVec2Class ivec2j = ivec2a.IntVec2_GetRotatedMinus90Degrees();
	IntVec2Class ivec2k = ivec2b.IntVec2_GetRotatedMinus90Degrees();
	IntVec2Class ivec2l = ivec2c.IntVec2_GetRotatedMinus90Degrees(); // NOTE: If this line does not compile, check if you forgot to make this method "const"!
	VerifyTestResult( IsEqual( ivec2g, -2, 3 ),		"IntVec2::GetRotated90Degrees incorrect for (3,2)" );
	VerifyTestResult( IsEqual( ivec2h, -4, -5 ),	"IntVec2::GetRotated90Degrees incorrect for (-5,4)" );
	VerifyTestResult( IsEqual( ivec2i, 2, 1 ),		"IntVec2::GetRotated90Degrees incorrect for (1,-2)" );
	VerifyTestResult( IsEqual( ivec2j, 2, -3 ),		"IntVec2::GetRotatedMinus90Degrees incorrect for (3,2)" );
	VerifyTestResult( IsEqual( ivec2k, 4, 5 ),		"IntVec2::GetRotatedMinus90Degrees incorrect for (-5,4)" );
	VerifyTestResult( IsEqual( ivec2l, -2, -1 ),	"IntVec2::GetRotatedMinus90Degrees incorrect for (1,-2)" );

	ivec2a.IntVec2_Rotate90Degrees();
	ivec2b.IntVec2_RotateMinus90Degrees();
	VerifyTestResult( IsEqual( ivec2a, -2, 3 ),		"IntVec2::Rotate90Degrees incorrect for (3,2)" );
	VerifyTestResult( IsEqual( ivec2b, 4, 5 ),		"IntVec2::RotateMinus90Degrees incorrect for (-5,4)" );

	ivec2a.IntVec2_Rotate90Degrees();
	ivec2b.IntVec2_RotateMinus90Degrees();
	VerifyTestResult( IsEqual( ivec2a, -3, -2 ),	"IntVec2::Rotate90Degrees incorrect for (-2,3)" );
	VerifyTestResult( IsEqual( ivec2b, 5, -4 ),		"IntVec2::RotateMinus90Degrees incorrect for (4,5)" );

#endif
	return 25; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_LerpAndClamp()
{
#if defined( ENABLE_TestSet_MP1A3_LerpAndClamp )

	// Interpolate
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 3.5f, 7.9f, 0.25f ), 4.6f ), "Interpolate() incorrect for 25% of the way from 3.5 to 7.9" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 1.1f, -7.7f, 0.75f ), -5.5f), "Interpolate() incorrect for 75% of the way from 1.1 to -7.7" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 3.1f, 9999.f, 0.f ), 3.1f ), "Interpolate() incorrect for 0% of the way from 3.1 to 9999" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 9999.f, 5.2f, 1.f ), 5.2f ), "Interpolate() incorrect for 1000% of the way from 9999 to 5.2" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 100.f, 200.f, 1.5f ), 250.f ), "Interpolate() incorrect for 150% of the way from 100 to 200 (should extrapolate beyond to 250)" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 500.f, 300.f, -0.5f ), 600.f ), "Interpolate() incorrect for -50% of the way from 500 to 300 (should extrapolate prior to 600)" );
	VerifyTestResult( IsMostlyEqual( Function_InterpolateFloat( 42.f, 42.f, 0.61f ), 42.f ), "Interpolate() incorrect for 61% of the way from 42 to 42 (should be 42)" );

	// Range map
	VerifyTestResult( IsMostlyEqual( Function_RangeMapFloat( 10.f, 20.f, 100.f, 200.f, 12.f ), 120.f ), "RangeMap() incorrect for 12 in [10,20] -> 120 in [100,200]" );
	VerifyTestResult( IsMostlyEqual( Function_RangeMapFloat( 5.5f, 2.2f, -100.55f, -100.22f, 4.4f ), -100.44f ), "RangeMap() incorrect for 4.4 in [5.5,2.2] -> -100.44 in [-100.55,-100.22]" );
	VerifyTestResult( IsMostlyEqual( Function_RangeMapFloat( 50.f, 90.f, 22.f, 22.f, 71.f ), 22.f ), "RangeMap() incorrect for 71 in [50,90] -> 22 in [22,22]" );

	// Clamp
	VerifyTestResult( Function_ClampFloat( 3.2f, 2.1f, 5.4f ) == 3.2f, "Clamp() was incorrect for 3.2 in [2.1,5.4] - should not modify value" );
	VerifyTestResult( Function_ClampFloat( 9.1f, 11.2f, 13.6f ) == 11.2f, "Clamp() was incorrect for 9.1 in [11.2,13.6] - should return min=11.2" );
	VerifyTestResult( Function_ClampFloat( 3.8f, -5.1f, -3.1f ) == -3.1f, "Clamp() was incorrect for 3.8 in [-5.1,-3.1] - should return max=-3.1" );
	VerifyTestResult( Function_ClampFloatZeroToOne( 0.21f ) == 0.21f, "ClampZeroToOne() was incorrect for 0.21 -> 0.21" );
	VerifyTestResult( Function_ClampFloatZeroToOne( 2.37f ) == 1.f, "ClampZeroToOne() was incorrect for 2.37 -> 1.0" );
	VerifyTestResult( Function_ClampFloatZeroToOne( -0.13f ) == 0.f, "ClampZeroToOne() was incorrect for -0.13 -> 0.0" );

	// Round
	VerifyTestResult( Function_RoundDownToInt( 3.14f ) == 3, "RoundDownToInt() was incorrect for 3.14f -> 3 (int)" );
	VerifyTestResult( Function_RoundDownToInt( 5.94f ) == 5, "RoundDownToInt() was incorrect for 5.94f -> 5 (int)" );
	VerifyTestResult( Function_RoundDownToInt( -0.01f ) == -1, "RoundDownToInt() was incorrect for -0.01f -> -1 (int)" );
	VerifyTestResult( Function_RoundDownToInt( -7.12f ) == -8, "RoundDownToInt() was incorrect for -7.12f -> -8 (int)" );

#endif
	return 20; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_Turning2D()
{
#if defined( ENABLE_TestSet_MP1A3_Turning2D )

	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 10.5f, 11.7f ), 1.2f ), "GetShortestAngularDisplacement() incorrect for 10.5 -> 11.7 degrees (should be 1.2)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 351.3f, 356.7f ), 5.4f ), "GetShortestAngularDisplacement() incorrect for 351.3 -> 356.7 degrees (should be 5.4)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 7.f, -5.f ), -12.f ), "GetShortestAngularDisplacement() incorrect for 7 -> -5 (or 355) degrees (should be -12)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 32.6f, 11.2f ), -21.4f ), "GetShortestAngularDisplacement() incorrect for 32.6 -> 11.2 degrees (should be -21.4)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( -30.2f, -50.7f ), -20.5f ), "GetShortestAngularDisplacement() incorrect for -30.2 -> -50.7 degrees (should be -20.5)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 185.3f, 175.2f ), -10.1f ), "GetShortestAngularDisplacement() incorrect for 185.3 -> 175.2 degrees (should be -10.1)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 355.f, 3.f ), 8.f ), "GetShortestAngularDisplacement() incorrect for 355 -> 3 degrees (should be +8)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 89.9f, 270.1f ), -179.8f ), "GetShortestAngularDisplacement() incorrect for 89.9 -> 270.1 degrees (should be -179.8)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 90.1f, 269.9f ), 179.8f ), "GetShortestAngularDisplacement() incorrect for 90.1 -> 269.9 degrees (should be +179.8)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 31.f, 403.f ), 12.f ), "GetShortestAngularDisplacement() incorrect for 31 -> 403 (or 43) degrees (should be +12)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( 1085.f, 7.f ), 2.f ), "GetShortestAngularDisplacement() incorrect for 1085 (or 5) -> 7 degrees (should be +2)" );
	VerifyTestResult( IsMostlyEqual( Function_GetShortAngDisp( -725.f, -1077.f ), 8.f ), "GetShortestAngularDisplacement() incorrect for -725 (or -5 or 355) -> -1077 (or -357 or 3) degrees (should be +8)" );

	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 10.f, 50.f, 5.f ), 15.f ), "GetTurnedToward() incorrect for 10 -> 50 degrees, max turn of 5 (should be 15)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 60.3f, 20.2f, 1.1f ), 59.2f ), "GetTurnedToward() incorrect for 60.3 -> 20.2 degrees, max turn of 1.1 (should be 59.2)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 71.6f, 71.6f, 10.f ), 71.6f ), "GetTurnedToward() incorrect for 71.6 -> 71.6 degrees, max turn of 10 (should be 71.6)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 22.f, 35.f, 0.f ), 22.f ), "GetTurnedToward() incorrect for 22 -> 35 degrees, max turn of 0 (should be 22)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 14.f, 17.f, 5.f ), 17.f ), "GetTurnedToward() incorrect for 14 -> 17 degrees, max turn of 5 (should be 17)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( -7.f, 20.f, 10.f ), 3.f ), "GetTurnedToward() incorrect for -7 (or 353) -> 20 degrees, max turn of 10 (should be 3)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 89.9f, 270.1f, 20.f ), 69.9f), "GetTurnedToward() incorrect for 89.9 -> 270.1 degrees, max turn of 20 (should be 69.9)" );
	VerifyTestResult( IsMostlyEqual( Function_GetTurnedToward( 90.1f, 269.9f, 10.f ), 100.1f ), "GetTurnedToward() incorrect for 90.1 -> 269.9 degrees, max turn of 10 (should be 100.1)" );

	float trickyTurn = Function_GetTurnedToward( 723.f, 1081.f, 3.f );
	float trickyTurn2 = Function_GetTurnedToward( -3602.f, 3615.f, 10.f );
	VerifyTestResult( IsMostlyEqual( trickyTurn, 1081.f ) || IsMostlyEqual( trickyTurn, 1.f ), "GetTurnedToward() incorrect for 723 (or 3) -> 1081 (or 1) degrees, max turn of 3 (should be 1 or 1081)" );
	VerifyTestResult( IsMostlyEqual( trickyTurn2, 8.f ) || IsMostlyEqual( trickyTurn2, -3592.f ), "GetTurnedToward() incorrect for -3602 (or -2 or 358) -> 3615 (or 15) degrees, max turn of 10 (should be 8 or -3592)" );

#endif
	return 22; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A3_DotProduct2D()
{
#if defined( ENABLE_TestSet_MP1A3_DotProduct2D )

	Vec2 zero( 0.f, 0.f );
	Vec2 one( 1.f, 1.f );
	Vec2 xAxis( 1.f, 0.f );
	Vec2 yAxis( 0.f, 1.f );
	Vec2 iBasis1( 0.8f, 0.6f );
	Vec2 jBasis1( -0.6f, 0.8f );
	Vec2 iBasis2( -4.f, -3.f );
	Vec2 jBasis2( 3.f, -4.f );

	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( zero, zero ), 0.f ), "DotProduct2D() incorrect for (0,0) dot (0,0) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( one, zero ), 0.f ), "DotProduct2D() incorrect for (1,1) dot (0,0) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( zero, jBasis2 ), 0.f ), "DotProduct2D() incorrect for (0,0) dot (3,-4) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( xAxis, iBasis1 ), 0.8f ), "DotProduct2D() incorrect for (1,0) dot (.8,.6) = .8" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( iBasis1, xAxis ), 0.8f ), "DotProduct2D() incorrect for (.8,.6) dot (1,0) = .8" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( iBasis1, yAxis ), 0.6f ), "DotProduct2D() incorrect for (.8,.6) dot (0,1) = .6" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( xAxis, yAxis), 0.f ), "DotProduct2D() incorrect for (1,0) dot (0,1) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( iBasis2, jBasis2 ), 0.f ), "DotProduct2D() incorrect for (-4,-3) dot (3,-4) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( jBasis1, iBasis1 ), 0.f ), "DotProduct2D() incorrect for (-.6,.8) dot (.8,.6) = 0" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( jBasis2, jBasis2 ), 25.f ), "DotProduct2D() incorrect for (3,-4) dot (3,-4) = 25" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( jBasis1, jBasis1 ), 1.f ), "DotProduct2D() incorrect for (-.6,.8) dot (-.6,.8) = 1" );
	VerifyTestResult( IsMostlyEqual( Function_DotProduct2D( iBasis2, iBasis1 ), -5.f ), "DotProduct2D() incorrect for (-4,-3) dot (.8,.6) = -5" );

#endif
	return 12; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A3()
{
	RunTestSet( true, TestSet_MP1A3_AABB2Basics,		"MP1-A3: AABB2 Basics" );
	RunTestSet( true, TestSet_MP1A3_AABB2Methods,		"MP1-A3: AABB2 Methods" );
	RunTestSet( true, TestSet_MP1A3_IntVec2Basics,		"MP1-A3: IntVec2 Basics" );
	RunTestSet( true, TestSet_MP1A3_IntVec2Methods,		"MP1-A3: IntVec2 Methods" );
	RunTestSet( true, TestSet_MP1A3_LerpAndClamp,		"MP1-A3: Lerping & Clamping" );
	RunTestSet( true, TestSet_MP1A3_Turning2D,			"MP1-A3: Turning" );
	RunTestSet( true, TestSet_MP1A3_DotProduct2D,		"MP1-A3: 2D Dot Product" );
}


