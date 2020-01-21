//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A2.cpp
//
#include "Game/UnitTests_MP1A2.hpp"
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


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector3Class& vec3, float x, float y, float z )
{
	return IsMostlyEqual( vec3.x, x ) && IsMostlyEqual( vec3.y, y ) && IsMostlyEqual( vec3.z, z );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector3Class& vec3a, const Vector3Class& vec3b )
{
	return IsMostlyEqual( vec3a, vec3b.x, vec3b.y, vec3b.z );
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_MathUtils_Angles()
{
#if defined( ENABLE_TestSet_MP1A2_MathUtils_Angles )

	float radsFor30Degrees		= Function_ConvertDegreesToRadians( 30.f );
	float radsForNegDegrees		= Function_ConvertDegreesToRadians( -713.61f );
	float degreesFor3Rads		= Function_ConvertRadiansToDegrees( 3.f );
	float degreesForNegRads		= Function_ConvertRadiansToDegrees( -18.05f );

	float cos0		= Function_CosDegrees( 0.f );		float cos0Correct		= 1.f;
	float cos45		= Function_CosDegrees( 45.f );		float cos45Correct		= 0.70710678118654752440084436210485f;
	float cos90		= Function_CosDegrees( 90.f );		float cos90Correct		= 0.f;
	float cos180	= Function_CosDegrees( 180.f );		float cos180Correct		= -1.f;
	float cos210	= Function_CosDegrees( 210.f );		float cos210Correct		= -0.86602540378443864676372317075294f;
	float cos270	= Function_CosDegrees( 270.f );		float cos270Correct		= 0.f;
	float cos370	= Function_CosDegrees( 370.f );		float cos370Correct		= 0.98480775301220805936674302458952f;
	float cosNeg10	= Function_CosDegrees( -10.f );		float cosNeg10Correct	= 0.98480775301220805936674302458952f;
	float cosNeg370 = Function_CosDegrees( -370.f );	float cosNeg370Correct	= 0.98480775301220805936674302458952f;

	float sin0		= Function_SinDegrees( 0.f );		float sin0Correct		= 0.f;
	float sin45		= Function_SinDegrees( 45.f );		float sin45Correct		= 0.70710678118654752440084436210485f;
	float sin90		= Function_SinDegrees( 90.f );		float sin90Correct		= 1.f;
	float sin180	= Function_SinDegrees( 180.f );		float sin180Correct		= 0.f;
	float sin210	= Function_SinDegrees( 210.f );		float sin210Correct		= -0.5f;
	float sin270	= Function_SinDegrees( 270.f );		float sin270Correct		= -1.f;
	float sin370	= Function_SinDegrees( 370.f );		float sin370Correct		=  0.17364817766693034885171662676931f;
	float sinNeg10	= Function_SinDegrees( -10.f );		float sinNeg10Correct	= -0.17364817766693034885171662676931f;
	float sinNeg370 = Function_SinDegrees( -370.f );	float sinNeg370Correct	= -0.17364817766693034885171662676931f;

	bool isCosDegreesCorrect = 
		IsMostlyEqual( cos0,		cos0Correct ) &&
		IsMostlyEqual( cos45,		cos45Correct ) &&
		IsMostlyEqual( cos90,		cos90Correct ) &&
		IsMostlyEqual( cos180,		cos180Correct ) &&
		IsMostlyEqual( cos210,		cos210Correct ) &&
		IsMostlyEqual( cos270,		cos270Correct ) &&
		IsMostlyEqual( cos370,		cos370Correct ) &&
		IsMostlyEqual( cosNeg10,	cosNeg10Correct ) &&
		IsMostlyEqual( cosNeg370,	cosNeg370Correct );

	bool isSinDegreesCorrect = 
		IsMostlyEqual( sin0,		sin0Correct ) &&
		IsMostlyEqual( sin45,		sin45Correct ) &&
		IsMostlyEqual( sin90,		sin90Correct ) &&
		IsMostlyEqual( sin180,		sin180Correct ) &&
		IsMostlyEqual( sin210,		sin210Correct ) &&
		IsMostlyEqual( sin270,		sin270Correct ) &&
		IsMostlyEqual( sin370,		sin370Correct ) &&
		IsMostlyEqual( sinNeg10,	sinNeg10Correct ) &&
		IsMostlyEqual( sinNeg370,	sinNeg370Correct );

	VerifyTestResult( IsMostlyEqual( radsFor30Degrees, 0.523598776f ),	"ConvertDegreesToRadians(30 deg) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( radsForNegDegrees, -12.4548435f ),	"ConvertDegreesToRadians(-713.61 deg) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( degreesFor3Rads, 171.88733854f ),	"ConvertRadiansToDegrees(3 rad) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( degreesForNegRads, -1034.18872f ),	"ConvertRadiansToDegrees(-18.05 rad) failed / was incorrect" );
	VerifyTestResult( isCosDegreesCorrect,		"CosDegrees() failed / was incorrect" );
	VerifyTestResult( isSinDegreesCorrect,		"SinDegrees() failed / was incorrect" );

	float atan2DegreesTest1 = Function_Atan2Degrees( 9.f, 9.f );
	float atan2DegreesTest2 = Function_Atan2Degrees( 0.f, -1.f );
	float atan2DegreesTest3 = Function_Atan2Degrees( -3.f, 4.f );
	float atan2DegreesTest4 = Function_Atan2Degrees( -0.4f, -0.3f );
	bool isAtan2DegreesTest3Correct = IsMostlyEqual( atan2DegreesTest3, -36.8698959f ) || IsMostlyEqual( atan2DegreesTest3, 323.130104f );
	bool isAtan2DegreesTest4Correct = IsMostlyEqual( atan2DegreesTest4, -126.869896f ) || IsMostlyEqual( atan2DegreesTest4, 233.130104f );

	VerifyTestResult( IsMostlyEqual( atan2DegreesTest1, 45.f ), "Atan2Degrees(y=9,x=9) failed - should be 45 degrees" );
	VerifyTestResult( IsMostlyEqual( atan2DegreesTest2, 180.f ), "Atan2Degrees(y=0,x=-1) failed - should be 180 degrees" );
	VerifyTestResult( isAtan2DegreesTest3Correct, "Atan2Degrees(y=-3,x=4) failed - should be about -36.9 (or +323.1) degrees" );
	VerifyTestResult( isAtan2DegreesTest4Correct, "Atan2Degrees(y=-.4,x=-.3) failed - should be about -126.9 (or +233.1) degrees" );

#endif
	return 10; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_MathUtils_Queries()
{
#if defined( ENABLE_TestSet_MP1A2_MathUtils_Queries )

	Vector2Class posA( -3.1f, 4.1f );
	Vector2Class posB( 4.9f, -1.9f ); // Points A&B are 10 apart (dx=8, dy=6)
	Vector2Class posC( -0.1f, 0.3f );
	Vector2Class posD( 0.4f, -0.9f ); // Points C&D are 1.3 apart (dx=.5, dy=1.2)

	VerifyTestResult( IsMostlyEqual( Function_GetDistance2D( posA, posB ), 10.f ), "GetDistance( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance2D( posB, posA ), 10.f ), "GetDistance( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance2D( posC, posD ), 1.3f ), "GetDistance( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance2D( posD, posC ), 1.3f ), "GetDistance( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared2D( posA, posB ), 100.f ), "GetDistanceSquared( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared2D( posB, posA ), 100.f ), "GetDistanceSquared( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared2D( posC, posD ), 1.69f ), "GetDistanceSquared( vec2, vec2 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared2D( posD, posC ), 1.69f ), "GetDistanceSquared( vec2, vec2 ) failed / was incorrect" );

 	Vector3Class posE( 6.0f, 7.3f, -8.6f );
 	Vector3Class posF( 6.2f, 7.0f, -8.0f ); // Points E&F are .7 apart (dx=.2, dy=.3, dz=.6)
	Vector3Class posG( 0.1f, -0.2f, 0.5f );
	Vector3Class posH( 0.0f, 0.2f, -0.3f ); // Points G&H are .9 apart (dx=.1, dy=.4, dz=.8)

	VerifyTestResult( IsMostlyEqual( Function_GetDistance3D( posE, posF ), 0.7f ), "GetDistance( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance3D( posF, posE ), 0.7f ), "GetDistance( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance3D( posG, posH ), 0.9f ), "GetDistance( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistance3D( posH, posG ), 0.9f ), "GetDistance( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared3D( posE, posF ), 0.49f ), "GetDistanceSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared3D( posF, posE ), 0.49f ), "GetDistanceSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared3D( posG, posH ), 0.81f ), "GetDistanceSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceSquared3D( posH, posG ), 0.81f ), "GetDistanceSquared( vec3, vec3 ) failed / was incorrect" );

	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXY3D( posE, posF ), 0.360555172f ), "GetDistanceXY( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXY3D( posF, posE ), 0.360555172f ), "GetDistanceXY( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXY3D( posG, posH ), 0.412310570f ), "GetDistanceXY( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXY3D( posH, posG ), 0.412310570f ), "GetDistanceXY( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXYSquared3D( posE, posF ), 0.13f ), "GetDistanceXYSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXYSquared3D( posF, posE ), 0.13f ), "GetDistanceXYSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXYSquared3D( posG, posH ), 0.17f ), "GetDistanceXYSquared( vec3, vec3 ) failed / was incorrect" );
	VerifyTestResult( IsMostlyEqual( Function_GetDistanceXYSquared3D( posH, posG ), 0.17f ), "GetDistanceXYSquared( vec3, vec3 ) failed / was incorrect" );

	Vector2Class discACenter( 0.3f, 0.8f );
	Vector2Class discBCenter( 3.3f, -3.2f ); // Discs A&B are 5 apart (dx=4, dy=3)

	// These three tests have combined radii > 5, therefore A&B should overlap here
	VerifyTestResult( Function_DoDiscsOverlap( discACenter, 3.1f, discBCenter, 2.1f ), "DoDiscsOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoDiscsOverlap( discACenter, 1.1f, discBCenter, 4.1f ), "DoDiscsOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoDiscsOverlap( discBCenter, 0.1f, discACenter, 5.1f ), "DoDiscsOverlap() should have overlapped (but didn't)" );

	// These three tests have combined radii < 5, therefore A&B should not overlap here
	VerifyTestResult( !Function_DoDiscsOverlap( discACenter, 2.9f, discBCenter, 1.9f ), "DoDiscsOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoDiscsOverlap( discACenter, 0.9f, discBCenter, 3.9f ), "DoDiscsOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoDiscsOverlap( discBCenter, 0.1f, discACenter, 4.8f ), "DoDiscsOverlap() should not overlap (but did)" );

	Vector2Class discCCenter( 0.08f, -0.02f );
	Vector2Class discDCenter( -0.04f, 0.03f ); // Discs C&D are .13 apart (dx=.12, dy=.05)

	// These three tests have combined radii > .13, therefore C&D should overlap here
	VerifyTestResult( Function_DoDiscsOverlap( discCCenter, 0.09f, discDCenter, 0.05f ), "DoDiscsOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoDiscsOverlap( discCCenter, 0.01f, discDCenter, 0.13f ), "DoDiscsOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoDiscsOverlap( discDCenter, 0.12f, discCCenter, 0.02f ), "DoDiscsOverlap() should have overlapped (but didn't)" );

	// These three tests have combined radii < .13, therefore C&D should not overlap here
	VerifyTestResult( !Function_DoDiscsOverlap( discCCenter, 0.09f, discDCenter, 0.03f ), "DoDiscsOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoDiscsOverlap( discCCenter, 0.01f, discDCenter, 0.11f ), "DoDiscsOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoDiscsOverlap( discDCenter, 0.10f, discCCenter, 0.02f ), "DoDiscsOverlap() should not overlap (but did)" );

	Vector3Class sphereACenter( 1.5f, 50.7f, -3.5f );
	Vector3Class sphereBCenter( -0.5f, 53.7f, 2.5f ); // Spheres A&B are 7 apart (dx=2, dy=3, dz=6)

	// These three tests have combined radii > 7, therefore A&B should overlap here
	VerifyTestResult( Function_DoSpheresOverlap( sphereACenter, 5.6f, sphereBCenter, 1.5f ),	"DoSpheresOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoSpheresOverlap( sphereACenter, 1.6f, sphereBCenter, 5.5f ),	"DoSpheresOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoSpheresOverlap( sphereBCenter, 2.55f, sphereACenter, 4.55f ),	"DoSpheresOverlap() should have overlapped (but didn't)" );

	// These three tests have combined radii < 7, therefore A&B should not overlap here
	VerifyTestResult( !Function_DoSpheresOverlap( sphereACenter, 1.5f, sphereBCenter, 5.4f ),	"DoSpheresOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoSpheresOverlap( sphereACenter, 5.5f, sphereBCenter, 1.4f ),	"DoSpheresOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoSpheresOverlap( sphereBCenter, 2.45f, sphereACenter, 4.45f ),	"DoSpheresOverlap() should not overlap (but did)" );

	Vector3Class sphereCCenter( 2.45f, 0.1f, -0.2f );
	Vector3Class sphereDCenter( 2.55f, -0.3f, 0.6f ); // Spheres C&D are .9 apart (dx=.1, dy=.4, dz=.8)

	// These three tests have combined radii > .9, therefore C&D should overlap here
	VerifyTestResult( Function_DoSpheresOverlap( sphereCCenter, 0.1f, sphereDCenter, 0.9f ), "DoSpheresOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoSpheresOverlap( sphereCCenter, 0.6f, sphereDCenter, 0.4f ), "DoSpheresOverlap() should have overlapped (but didn't)" );
	VerifyTestResult( Function_DoSpheresOverlap( sphereDCenter, 0.5f, sphereCCenter, 0.5f ), "DoSpheresOverlap() should have overlapped (but didn't)" );

	// These three tests have combined radii < .9, therefore C&D should not overlap here
	VerifyTestResult( !Function_DoSpheresOverlap( sphereCCenter, 0.4f, sphereDCenter, 0.4f ), "DoSpheresOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoSpheresOverlap( sphereCCenter, 0.6f, sphereDCenter, 0.2f ), "DoSpheresOverlap() should not overlap (but did)" );
	VerifyTestResult( !Function_DoSpheresOverlap( sphereDCenter, 0.3f, sphereCCenter, 0.5f ), "DoSpheresOverlap() should not overlap (but did)" );

#endif
	return 48; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_Vec2_Methods()
{
#if defined( ENABLE_TestSet_MP1A2_Vec2_Methods )

	// Vec2::MakeFromPolar static methods
	Vector2Class a( 4.f, 3.f );
	Vector2Class b( 8.f, -6.f );
	Vector2Class c( -0.06f, -0.08f );
	Vector2Class d( 0.f, 1.f );
	Vector2Class e = Vector2Class::Vec2_MakeFromPolarRadians( 5.1f, 9.3f );
	Vector2Class f = Vector2Class::Vec2_MakeFromPolarDegrees( 160.3f, 2.7f );
	Vector2Class g = Vector2Class::Vec2_MakeFromPolarRadians( 3.9f );
	Vector2Class h = Vector2Class::Vec2_MakeFromPolarDegrees( 245.7f );

	float eCorrectX = -2.5419704709925023961033282004531f;
	float eCorrectY =  0.9101571977423216172547009490633f;
	float fCorrectX =  3.5151930072307192388813526422395f;
	float fCorrectY = -8.6100765456479103615991601021672f;
	float gCorrectX = -0.72593230420014012937233048461435f;
	float gCorrectY = -0.68776615918397381809088812537869f;
	float hCorrectX = -0.41151435860510877405343473217572f;
	float hCorrectY = -0.91140327663544524821332779863426f;

	VerifyTestResult( IsMostlyEqual( e, fCorrectX, fCorrectY ), "Vec2::MakeFromPolarRadians() failed for radians=5.1, length=9.3" );
	VerifyTestResult( IsMostlyEqual( f, eCorrectX, eCorrectY ), "Vec2::MakeFromPolarDegrees() failed for degrees=160.3, length=2.7" );
	VerifyTestResult( IsMostlyEqual( g, gCorrectX, gCorrectY ), "Vec2::MakeFromPolarRadians() failed for radians=3.9, length=default(1)" );
	VerifyTestResult( IsMostlyEqual( h, hCorrectX, hCorrectY ), "Vec2::MakeFromPolarDegrees() failed for degrees=245.7, length=default(1)" );

	// Vec2::GetLength methods
	float aLength = a.Vec2_GetLength();
	float bLength = b.Vec2_GetLength();
	float cLength = c.Vec2_GetLength();
	float aLengthSquared = a.Vec2_GetLengthSquared();
	float bLengthSquared = b.Vec2_GetLengthSquared();
	float cLengthSquared = c.Vec2_GetLengthSquared();
	float dLengthSquared = d.Vec2_GetLengthSquared();

	float aLengthCorrect = 5.f;
	float bLengthCorrect = 10.f;
	float cLengthCorrect = 0.1f;
	float aLengthSquaredCorrect = 25.f;
	float bLengthSquaredCorrect = 100.f;
	float cLengthSquaredCorrect = 0.01f;
	float dLengthSquaredCorrect = 1.f;

	VerifyTestResult( IsMostlyEqual( aLength, aLengthCorrect ), "Vec2::GetLength() is wrong for positive x,y" );
	VerifyTestResult( IsMostlyEqual( bLength, bLengthCorrect ), "Vec2::GetLength() is wrong for positive x, negative y" );
	VerifyTestResult( IsMostlyEqual( cLength, cLengthCorrect ), "Vec2::GetLength() is wrong for negative x,y" );
	VerifyTestResult( IsMostlyEqual( aLengthSquared, aLengthSquaredCorrect ), "Vec2::GetLengthSquared() is wrong for positive x,y" );
	VerifyTestResult( IsMostlyEqual( bLengthSquared, bLengthSquaredCorrect ), "Vec2::GetLengthSquared() is wrong for mixed x,y" );
	VerifyTestResult( IsMostlyEqual( cLengthSquared, cLengthSquaredCorrect ), "Vec2::GetLengthSquared() is wrong for negative x,y" );
	VerifyTestResult( IsMostlyEqual( dLengthSquared, dLengthSquaredCorrect ), "Vec2::GetLengthSquared() is wrong for vec2(0,1)" );

	// Vec2::GetAngle methods
	float aRadians = a.Vec2_GetAngleRadians();
	float cRadians = c.Vec2_GetAngleRadians();
	float dRadians = d.Vec2_GetAngleRadians();
	float aDegrees = a.Vec2_GetAngleDegrees();
	float cDegrees = c.Vec2_GetAngleDegrees();
	float dDegrees = d.Vec2_GetAngleDegrees();

	float aRadiansCorrect = 0.64350110879328438680280922871732f;
	float cRadiansCorrect = -2.214297435588181006034130920357f;
	float dRadiansCorrect = 1.5707963267948966192313216916398f;
	float aDegreesCorrect = 36.869897645844021296855612559093f;
	float cDegreesCorrect = -126.86989764584402129685561255909f;
	float dDegreesCorrect = 90.f;

	VerifyTestResult( IsMostlyEqual( aRadians, aRadiansCorrect ), "Vec2::GetAngleRadians() is wrong for positive x,y" );
	VerifyTestResult( IsMostlyEqual( cRadians, cRadiansCorrect ), "Vec2::GetAngleRadians() is wrong for negative x,y" );
	VerifyTestResult( IsMostlyEqual( dRadians, dRadiansCorrect ), "Vec2::GetAngleRadians() is wrong for x=0 (vertical)" );
	VerifyTestResult( IsMostlyEqual( aDegrees, aDegreesCorrect ), "Vec2::GetAngleDegrees() is wrong for positive x,y" );
	VerifyTestResult( IsMostlyEqual( cDegrees, cDegreesCorrect ), "Vec2::GetAngleDegrees() is wrong for negative x,y" );
	VerifyTestResult( IsMostlyEqual( dDegrees, dDegreesCorrect ), "Vec2::GetAngleDegrees() is wrong for x=0 (vertical)" );

	// Vec2::GetRotated methods
	Vector2Class aRotated90				= a.Vec2_GetRotated90Degrees();
	Vector2Class cRotated90				= c.Vec2_GetRotated90Degrees();
	Vector2Class bRotatedNeg90			= b.Vec2_GetRotatedMinus90Degrees();
	Vector2Class cRotatedNeg90			= c.Vec2_GetRotatedMinus90Degrees();
	Vector2Class aRotated1Rad			= a.Vec2_GetRotatedRadians( 1.f );
	Vector2Class bRotated9Rad			= b.Vec2_GetRotatedRadians( 9.f );
	Vector2Class cRotatedNegRad			= c.Vec2_GetRotatedRadians( -7.31f );
	Vector2Class aRotated30				= a.Vec2_GetRotatedDegrees( 30.f );
	Vector2Class bRotatedNeg205			= b.Vec2_GetRotatedDegrees( -205.f );
	Vector2Class cRotated555			= c.Vec2_GetRotatedDegrees( 555.555f );

	float aRot90CorrectX		= -3.f;			float aRot90CorrectY		= 4.f;
	float cRot90CorrectX		= 0.08f;		float cRot90CorrectY		= -0.06f;
	float bRotNeg90CorrectX		= -6.f;			float bRotNeg90CorrectY		= -8.f;
	float cRotNeg90CorrectX		= -0.08f;		float cRotNeg90CorrectY		= 0.06f;
	float aRot1RadCorrectX		= -0.36320373f;	float aRot1RadCorrectY		= 4.9867908f;
	float bRot9RadCorrectX		= -4.81633118f;	float bRot9RadCorrectY		= 8.7637294f;
	float cRotNegRadCorrectX	= -0.09950520f;	float cRotNegRadCorrectY	= 0.00993552806f;
	float aRot30CorrectX		= 1.9641016f;	float aRot30CorrectY		= 4.5980762f;
	float bRotNeg205CorrectX	= -4.7147527f;	float bRotNeg205CorrectY	= 8.8187928f;
	float cRot555CorrectX		= 0.036349393f;	float cRot555CorrectY		= 0.09315966f;

	VerifyTestResult( IsMostlyEqual( aRotated90,	 aRot90CorrectX,	 aRot90CorrectY		), "Vec2::GetRotated90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( cRotated90,	 cRot90CorrectX,	 cRot90CorrectY		), "Vec2::GetRotated90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( bRotatedNeg90,  bRotNeg90CorrectX,  bRotNeg90CorrectY	), "Vec2::GetRotatedMinus90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( cRotatedNeg90,  cRotNeg90CorrectX,  cRotNeg90CorrectY	), "Vec2::GetRotatedMinus90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( aRotated1Rad,	 aRot1RadCorrectX,	 aRot1RadCorrectY	), "Vec2::GetRotatedRadians(1) was incorrect" );
	VerifyTestResult( IsMostlyEqual( bRotated9Rad,	 bRot9RadCorrectX,	 bRot9RadCorrectY	), "Vec2::GetRotatedRadians(9) was incorrect" );
	VerifyTestResult( IsMostlyEqual( cRotatedNegRad, cRotNegRadCorrectX, cRotNegRadCorrectY ), "Vec2::GetRotatedRadians(-7.31) was incorrect" );
	VerifyTestResult( IsMostlyEqual( aRotated30,	 aRot30CorrectX,	 aRot30CorrectY		), "Vec2::GetRotatedDegrees(30) was incorrect" );
	VerifyTestResult( IsMostlyEqual( bRotatedNeg205, bRotNeg205CorrectX, bRotNeg205CorrectY ), "Vec2::GetRotatedDegrees(-205) was incorrect" );
	VerifyTestResult( IsMostlyEqual( cRotated555,	 cRot555CorrectX,	 cRot555CorrectY	), "Vec2::GetRotatedDegrees(555) was incorrect" );

	// Vec2::Polar methods
	Vector2Class j( 30.f, -40.f );
	Vector2Class k( -8.f, 6.f );
	Vector2Class l( 1000.f, 0.f );
	Vector2Class m( -2.0f, -1.5f );
	Vector2Class n( -3.1f, -4.3f );
	Vector2Class o( 200.f, 100.f );
	Vector2Class p( 1.3f, 2.1f );
	Vector2Class q( -2.3f, 3.1f );
	Vector2Class r( 3.3f, -4.1f );
	Vector2Class s( -4.3f, -5.1f );
	Vector2Class t( 7.3f, -8.1f );
	Vector2Class u( -8.3f, -9.1f );
	Vector2Class v( 5.3f, 6.1f );
	Vector2Class w( -6.3f, 7.1f );
	j.Vec2_SetAngleRadians( 2.4980915447965088516598341545622f );
	k.Vec2_SetAngleRadians( -666.86989764584402129685561255909f );
	l.Vec2_SetAngleDegrees( 36.869897645844021296855612559093f );
	m.Vec2_SetAngleDegrees( -233.13010235415597870314438744091f );
 	n.Vec2_SetPolarRadians( -8.5f, 10.f );
 	o.Vec2_SetPolarDegrees( 30.f, 100.f );
	p.Vec2_Rotate90Degrees();
	q.Vec2_RotateMinus90Degrees();
	r.Vec2_Rotate90Degrees();
	s.Vec2_RotateMinus90Degrees();
	t.Vec2_RotateRadians( 3.1415926535897932384626433832795f / 4.f );
	u.Vec2_RotateRadians( -10.7f );
	v.Vec2_RotateDegrees( 30.f );
	w.Vec2_RotateDegrees( -501.3f );

	float JCorrectX			= -40.f;		float JCorrectY			= 30.f;
	float KCorrectX			= 6.58287268f;	float KCorrectY			= -7.52766812f;
	float LCorrectX			= 800.f;		float LCorrectY			= 600.f;
	float MCorrectX			= -1.5f;		float MCorrectY			= 2.0f;
	float NCorrectX			= -6.0201190f;	float NCorrectY			= -7.98487112f;
	float OCorrectX			= 86.6025403f;	float OCorrectY			= 50.f;
	float pCorrectX			= -2.1f;		float pCorrectY			= 1.3f;
	float qCorrectX			= 3.1f;			float qCorrectY			= 2.3f;
	float rCorrectX			= 4.1f;			float rCorrectY			= 3.3f;
	float sCorrectX			= -5.1f;		float sCorrectY			= 4.3f;
	float TCorrectX			= 10.8894444f;	float TCorrectY			= -0.56568542495f;
	float UCorrectX			= 11.1230797f;	float UCorrectY			= -5.2893381714f;
	float VCorrectX			= 1.53993464f;	float VCorrectY			= 7.9327549631f;
	float WCorrectX			= 9.35593443f;	float WCorrectY			= -1.6020271572f;

	VerifyTestResult( IsMostlyEqual( j, JCorrectX, JCorrectY ), "Vec2::SetAngleRadians(2.498092) was incorrect" );
	VerifyTestResult( IsMostlyEqual( k, KCorrectX, KCorrectY ), "Vec2::SetAngleRadians(-666.869898) was incorrect" );
	VerifyTestResult( IsMostlyEqual( l, LCorrectX, LCorrectY ), "Vec2::SetAngleDegrees(36.869898) was incorrect" );
	VerifyTestResult( IsMostlyEqual( m, MCorrectX, MCorrectY ), "Vec2::SetAngleDegrees(-233.130102) was incorrect" );
	VerifyTestResult( IsMostlyEqual( n, NCorrectX, NCorrectY ), "Vec2::SetPolarRadians(radians=-8.5,len=10) was incorrect" );
	VerifyTestResult( IsMostlyEqual( o, OCorrectX, OCorrectY ), "Vec2::SetPolarDegrees(degrees=30,len=100) was incorrect" );
	VerifyTestResult( IsMostlyEqual( p, pCorrectX, pCorrectY ), "Vec2::Rotate90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( q, qCorrectX, qCorrectY ), "Vec2::RotateMinus90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( r, rCorrectX, rCorrectY ), "Vec2::Rotate90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( s, sCorrectX, sCorrectY ), "Vec2::RotateMinus90Degrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( t, TCorrectX, TCorrectY ), "Vec2::RotateRadians(pi/4) was incorrect" );
	VerifyTestResult( IsMostlyEqual( u, UCorrectX, UCorrectY ), "Vec2::RotateRadians(-10.7) was incorrect" );
	VerifyTestResult( IsMostlyEqual( v, VCorrectX, VCorrectY ), "Vec2::RotateDegrees(30) was incorrect" );
	VerifyTestResult( IsMostlyEqual( w, WCorrectX, WCorrectY ), "Vec2::RotateDegrees(-501.3) was incorrect" );

	// Vec2::Normalize methods
	Vector2Class aa( 0.8f, 0.6f );		// Length is 1.0
	Vector2Class bb( -606.f, -808.f );	// Length is 1010
	Vector2Class cc( -6.f, 8.f );		// Length is 10
	Vector2Class dd( 66.f, -88.f );		// Length is 110
	Vector2Class aaNotClamped	= aa.Vec2_GetClamped( 1.1f );
	Vector2Class aaClamped		= aa.Vec2_GetClamped( 0.1f );
	Vector2Class bbNotClamped	= bb.Vec2_GetClamped( 1020.f );
	Vector2Class bbClamped		= bb.Vec2_GetClamped( 1000.f );
	Vector2Class aNormalized	= a.Vec2_GetNormalized();
	Vector2Class cNormalized	= c.Vec2_GetNormalized();
	aa.Vec2_SetLength( 20.f );
	bb.Vec2_SetLength( 10.f );
	cc.Vec2_ClampLength( 11.f ); // Should do nothing to cc, still (-6,8)
	dd.Vec2_ClampLength( 100.f ); // Should clamp from 110 to 100, now (60,-80)
	c.Vec2_Normalize();
	float aOldLength = a.Vec2_NormalizeAndGetLength();
	float bOldLength = b.Vec2_NormalizeAndGetLength();
	float aNewLength = a.Vec2_GetLength();
	float bNewLength = b.Vec2_GetLength();
	float cNewLength = c.Vec2_GetLength();

	float aaNoClampCorrectX = 0.8f;		float aaNoClampCorrectY = 0.6f;
	float aaClampedCorrectX = 0.08f;	float aaClampedCorrectY = 0.06f;
	float bbNoClampCorrectX = -606.f;	float bbNoClampCorrectY = -808.f;
	float bbClampedCorrectX = -600.f;	float bbClampedCorrectY = -800.f;
	float ccNoClampCorrectX = -6.f;		float ccNoClampCorrectY = 8.f;
	float ddClampedCorrectX = 60.f;		float ddClampedCorrectY = -80.f;
	float aNormCorrectX = 0.8f;			float aNormCorrectY = 0.6f;
	float cNormCorrectX = -0.6f;		float cNormCorrectY = -0.8f;
	float aCorrectX = 0.8f;				float aCorrectY = 0.6f;
	float cCorrectX = -0.6f;			float cCorrectY = -0.8f;
	float aOldLenCorrect = 5.f;
	float bOldLenCorrect = 10.f;
	float aNewLenCorrect = 1.f;
	float bNewLenCorrect = 1.f;
	float cNewLenCorrect = 1.f;

	VerifyTestResult( IsMostlyEqual( aaNotClamped, aaNoClampCorrectX, aaNoClampCorrectY ),	"Vec2::GetClamped() clamped when it shouldn't have" );
	VerifyTestResult( IsMostlyEqual( aaClamped, aaClampedCorrectX, aaClampedCorrectY ),		"Vec2::GetClamped() did not clamp correctly" );
	VerifyTestResult( IsMostlyEqual( bbNotClamped, bbNoClampCorrectX, bbNoClampCorrectY ),	"Vec2::GetClamped() clamped when it shouldn't have" );
	VerifyTestResult( IsMostlyEqual( bbClamped, bbClampedCorrectX, bbClampedCorrectY ),		"Vec2::GetClamped() did not clamp correctly" );
	VerifyTestResult( IsMostlyEqual( cc, ccNoClampCorrectX, ccNoClampCorrectY ),			"Vec2::GetClamped() clamped when it shouldn't have" );
	VerifyTestResult( IsMostlyEqual( dd, ddClampedCorrectX, ddClampedCorrectY ),			"Vec2::GetClamped() did not clamp correctly" );

	VerifyTestResult( IsMostlyEqual( aNormalized, aNormCorrectX, aNormCorrectY ),			"Vec2::GetNormalized() was incorrect" );
	VerifyTestResult( IsMostlyEqual( cNormalized, cNormCorrectX, cNormCorrectY ),			"Vec2::GetNormalized() was incorrect" );
	VerifyTestResult( IsMostlyEqual( c, cCorrectX, cCorrectY ),								"Vec2::Normalize() did not normalize correctly" );
	VerifyTestResult( IsMostlyEqual( a, aCorrectX, aCorrectY ),								"Vec2::NormalizeAndGetLength() did not normalize correctly" );
	VerifyTestResult( IsMostlyEqual( aOldLength, aOldLenCorrect ),							"Vec2::NormalizeAndGetLength() did not return previous length" );
	VerifyTestResult( IsMostlyEqual( bOldLength, bOldLenCorrect ),							"Vec2::NormalizeAndGetLength() did not return previous length" );
	VerifyTestResult( IsMostlyEqual( aNewLength, aNewLenCorrect ),							"Vec2::NormalizeAndGetLength() failed to make vec2 length=1" );
	VerifyTestResult( IsMostlyEqual( bNewLength, bNewLenCorrect ),							"Vec2::NormalizeAndGetLength() failed to make vec2 length=1" );
	VerifyTestResult( IsMostlyEqual( cNewLength, cNewLenCorrect ),							"Vec2::Normalize() failed to make vec2 length=1" );

#endif	
	return 56; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_Vec3_Basics()
{
#if defined( ENABLE_TestSet_MP1A2_Vec3_Basics )

	Vector3Class vec3a;										// default constructor
	Vector3Class vec3b( 1.4f, -1.6f, 0.2f );				// explicit constructor
	Vector3Class vec3c = Vector3Class( -1.3f, 1.7f, 0.3f );	// explicit constructor (per C++ standard)
	Vector3Class vec3d( vec3b );							// copy constructor
	Vector3Class vec3e = vec3c;								// copy constructor (per C++ standard)

	VerifyTestResult( sizeof( Vector3Class ) == 12, "sizeof(Vec3) was not 12 bytes" );
	VerifyTestResult( IsMostlyEqual( vec3b, 1.4f, -1.6f, 0.2f ), "Vec3( x, y, z ) : explicit constructor failed to assign x, y, and/or z" );
	VerifyTestResult( IsMostlyEqual( vec3d, 1.4f, -1.6f, 0.2f ), "Vec3( Vec3 ) : copy constructor failed to copy x, y, and/or z" );

	Vector3Class vec3f( 3.5f, -1.5f, -7.1f );				// explicit constructor from x,y
	Vector3Class vec3g = Vector3Class( -0.5f, 1.0f, 0.1f );	// explicit constructor from x,y

	Vector3Class vec3h = vec3f + vec3g;		// operator+ (Vec3 + Vec3)
	Vector3Class vec3i = vec3f + vec3f;		// operator+ (Vec3 + Vec3), added with itself
	Vector3Class vec3j = vec3f - vec3g;		// operator- (Vec3 - Vec3)
	Vector3Class vec3k = vec3f - vec3f;		// operator- (Vec3 - Vec3), subtract from itself
	Vector3Class vec3l = vec3f * 0.2f;		// operator* (Vec3 * float)
	Vector3Class vec3m = 0.2f * vec3g;		// operator* (float * Vec3)
	Vector3Class vec3n = vec3f / 0.5f;		// operator/ (Vec3 / float)

	VerifyTestResult( IsMostlyEqual( vec3h, 3.0f, -0.5f, -7.f ),	"Vec3::operator+ : did not add (vec3 + vec3) correctly" );
	VerifyTestResult( IsMostlyEqual( vec3i, 7.0f, -3.0f, -14.2f ),	"Vec3::operator+ : did not add (vec3 + vec3) with itself correctly" );
	VerifyTestResult( IsMostlyEqual( vec3j, 4.0f, -2.5f, -7.2f ),	"Vec3::operator- : did not subtract (vec3 - vec3) correctly" );
	VerifyTestResult( IsMostlyEqual( vec3k, 0.0f, 0.0f, 0.0f ),		"Vec3::operator- : did not subtract (vec3 - vec3) from itself correctly" );
	VerifyTestResult( IsMostlyEqual( vec3l, 0.7f, -0.3f, -1.42f ),	"Vec3::operator* : did not scale (vec3 * float) correctly" );
	VerifyTestResult( IsMostlyEqual( vec3m, -0.1f, 0.2f, 0.02f ),	"Vec3::operator* : did not scale (float * vec3) correctly" );
	VerifyTestResult( IsMostlyEqual( vec3n, 7.0f, -3.0f, -14.2f ),	"Vec3::operator/ : did not divide (vec3 / float) correctly" );

	Vector3Class vec3o( 3.f, 4.f, 5.f );	// explicit constructor
	Vector3Class vec3p( 3.f, 4.f, 5.f );	// explicit constructor
	Vector3Class vec3q( 99.f, 4.f, 5.f );	// explicit constructor
	Vector3Class vec3r( 3.f, 99.f, 5.f );	// explicit constructor
	Vector3Class vec3s( 3.f, 4.f, 99.f );	// explicit constructor

	VerifyTestResult( vec3o == vec3p,		"Vec3::operator== : failed to detect match" );
	VerifyTestResult( !(vec3o != vec3p),	"Vec3::operator!= : failed to reject match" );

	VerifyTestResult( vec3o != vec3q,		"Vec3::operator!= : failed to detect x-mismatch" );
	VerifyTestResult( vec3o != vec3r,		"Vec3::operator!= : failed to detect y-mismatch" );
	VerifyTestResult( vec3o != vec3s,		"Vec3::operator!= : failed to detect z-mismatch" );

	VerifyTestResult( !(vec3o == vec3q),	"Vec3::operator== : failed to reject x-mismatch" );
	VerifyTestResult( !(vec3o == vec3r),	"Vec3::operator== : failed to reject y-mismatch" );
	VerifyTestResult( !(vec3o == vec3s),	"Vec3::operator== : failed to reject z-mismatch" );

	Vector3Class vec3t( 3.f, 4.f, 5.f );		// explicit constructor
	Vector3Class vec3u;							// default constructor
	vec3u = vec3t;								// operator= (assignment operator)
	vec3t += Vector3Class( 1.f, 2.f, 3.f );		// operator+= (add-and-assign operator)
	Vector3Class vec3v( 3.f, 4.f, 5.f );		// 
	vec3v -= Vector3Class( 2.f, 1.f, 3.f );		// operator-= (subtract-and-assign operator)
	Vector3Class vec3w( 3.f, 4.f, 5.f );		// 
	Vector3Class vec3aa( 5.f, 7.f, 9.f );		// 
	Vector3Class vec3bb( 1.f, 2.f, 3.f );		// 
	Vector3Class vec3cc( 8.f, 6.f, 4.f );		// 
	vec3w += vec3aa;							// 
	vec3aa -= vec3bb;							// 
	vec3bb *= 3.f;								// operator*= (uniform scale by float)
	vec3cc /= 2.f;								// operator/= (uniform scale by 1/float)

	VerifyTestResult( IsMostlyEqual( vec3u, 3.f, 4.f, 5.f ),	"Vec3::operator= : failed to copy correctly" );
	VerifyTestResult( IsMostlyEqual( vec3t, 4.f, 6.f, 8.f ),	"Vec3::operator+= : failed to add correctly" );
	VerifyTestResult( IsMostlyEqual( vec3v, 1.f, 3.f, 2.f ),	"Vec3::operator-= : failed to subtract correctly" );
	VerifyTestResult( IsMostlyEqual( vec3w, 8.f, 11.f, 14.f ),	"Vec3::operator+= : failed to add existing Vec3" );
	VerifyTestResult( IsMostlyEqual( vec3aa, 4.f, 5.f, 6.f ),	"Vec3::operator-= : failed to subtract existing Vec3" );
	VerifyTestResult( IsMostlyEqual( vec3bb, 3.f, 6.f, 9.f ),	"Vec3::operator*= : failed to scale correctly" );
	VerifyTestResult( IsMostlyEqual( vec3cc, 4.f, 3.f, 2.f ),	"Vec3::operator/= : failed to divide correctly" );

#endif
	return 25; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_Vec3_Methods()
{
#if defined( ENABLE_TestSet_MP1A2_Vec3_Methods )

	Vector3Class vec3a( 0.2f, -0.3f, 0.6f ); // Length of 0.7
	Vector3Class vec3b( -1.1f, 4.4f, -8.8f ); // Length of 9.9

	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetLength(), 0.7f ),						"Vec3::GetLength() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetLength(), 9.9f ),						"Vec3::GetLength() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetLengthXY(), 0.360555f ),					"Vec3::GetLengthXY() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetLengthXY(), 4.535416f ),					"Vec3::GetLengthXY() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetLengthSquared(), 0.49f ),				"Vec3::GetLengthSquared() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetLengthSquared(), 98.01f ),				"Vec3::GetLengthSquared() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetLengthXYSquared(), 0.13f ),				"Vec3::GetLengthXYSquared() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetLengthXYSquared(), 20.57f ),				"Vec3::GetLengthXYSquared() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetAngleAboutZRadians(), -0.982793748f ),	"Vec3::GetAngleAboutZRadians() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetAngleAboutZRadians(), 1.81577504f ),		"Vec3::GetAngleAboutZRadians() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3a.Vec3_GetAngleAboutZDegrees(), -56.309932f ),		"Vec3::GetAngleAboutZDegrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3b.Vec3_GetAngleAboutZDegrees(), 104.036240f ),		"Vec3::GetAngleAboutZDegrees() was incorrect" );

	Vector3Class vec3c = vec3a.Vec3_GetRotatedAboutZRadians( 1.f );
	Vector3Class vec3d = vec3b.Vec3_GetRotatedAboutZRadians( -12.345f );
	Vector3Class vec3e = vec3a.Vec3_GetRotatedAboutZDegrees( 10.f );
	Vector3Class vec3f = vec3b.Vec3_GetRotatedAboutZDegrees( -1234.567f );

	// Make sure z is never affected by XY rotations
	VerifyTestResult( IsMostlyEqual( vec3c.z, vec3a.z ), "Vec3::GetRotatedAboutZRadians() should not affect vec3.z" );
	VerifyTestResult( IsMostlyEqual( vec3f.z, vec3b.z ), "Vec3::GetRotatedAboutZDegrees() should not affect vec3.z" );

	// Full rotational results check (changes on XY only, rotating "in 2D" about Z)
	Vector3Class vec3cCorrect( 0.360501766f, 0.00620349636f, 0.600000024f );
	Vector3Class vec3dCorrect( -2.03925061f, 4.05110550f, -8.80000019f );
	Vector3Class vec3eCorrect( 0.249056026f, -0.260712683f, 0.600000024f );
	Vector3Class vec3fCorrect( 2.88300228f, -3.50118518f, -8.80000019f );

	VerifyTestResult( IsMostlyEqual( vec3c, vec3cCorrect ), "Vec3::GetRotatedAboutZRadians() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3d, vec3dCorrect ), "Vec3::GetRotatedAboutZRadians() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3e, vec3eCorrect ), "Vec3::GetRotatedAboutZDegrees() was incorrect" );
	VerifyTestResult( IsMostlyEqual( vec3f, vec3fCorrect ), "Vec3::GetRotatedAboutZDegrees() was incorrect" );

#endif
	return 18; // Number of tests expected
}


//------------------------------------------------------------------------------------------------
void CountIntRngResult( int randomInt, int arraySize, int* resultsArray, int minLegal, int maxLegal )
{
	if( randomInt >= minLegal && randomInt <= maxLegal )
	{
		// Result in legal range; increment count in array
		++ resultsArray[ randomInt ];
	}
	else
	{
		++ resultsArray[ arraySize-1 ]; // N-1 spot in array is reserved for illegal "out of bounds" count
		static int timesSeenThisMessage = 0;
		if( timesSeenThisMessage < 3 )
		{
			printf( "\n  ERROR: An RNG function returned random integer %i - outside legal range [%i,%i]!", randomInt, minLegal, maxLegal );
			++ timesSeenThisMessage;
		}
	}
}


//------------------------------------------------------------------------------------------------
void VerifyAndReportIntRngTest( int arraySize, int* resultsArray, int minLegal, int maxLegal, int numRolls )
{
	int numValidResults = 1 + (maxLegal - minLegal);
	int averageCountPerResult = numRolls / numValidResults;
	int maxCountDeviation = averageCountPerResult / 5; // Allow up to +/- 20% variation on expectation
	int illegalResultCount = resultsArray[ arraySize-1 ];

	// Check if there were any illegal (out-of-range) results, e.g. returning 3 or 9 if we want a number in [4,7]
	VerifyTestResult( illegalResultCount == 0, "RNG function returned one or more illegal (out-of-range) numbers!" );
	if( illegalResultCount > 0 )
	{
		float illegalResultPercent = 100.f * static_cast< float >( illegalResultCount ) / static_cast< float >( numRolls );
		printf( "\n  Out of %i rolls, %i (%.0f%%) were outside the legal range [%i,%i] the RNG should have provided", numRolls, illegalResultCount, illegalResultPercent, minLegal, maxLegal );
	}

	// Check the result counts to see if all expected results were [roughly] evenly distributed
	int worstCountDeviation = 0;
	int worstCountDeviationIndex = 0;
	for( int resultIndex = minLegal; resultIndex <= maxLegal; ++ resultIndex )
	{
		int resultCount = resultsArray[ resultIndex ];
		int countDeviation = abs( resultCount - averageCountPerResult );
		if( countDeviation > worstCountDeviation )
		{
			worstCountDeviation = countDeviation;
			worstCountDeviationIndex = resultIndex;
		}
	}

	VerifyTestResult( worstCountDeviation <= maxCountDeviation, "RNG function returned a grossly uneven random distribution of numbers!" );
	if( worstCountDeviation > maxCountDeviation )
	{
		int worstCount = resultsArray[ worstCountDeviationIndex ];
		float worstDeviationPercent = 100.f * static_cast< float >( worstCountDeviation ) / static_cast< float >( averageCountPerResult );
		printf( "\n  Expected %i rolls per possible result (in [%i,%i]); result [%i] got %i rolls (%.0f%% deviation)", averageCountPerResult, minLegal, maxLegal, worstCountDeviationIndex, worstCount, worstDeviationPercent );
	}
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_RandomNumberGenerator()
{
#if defined( ENABLE_TestSet_MP1A2_RandomNumberGenerator )

	RngClass rng;

	// Test result buckets for Rng_GetRandomIntLessThan and Rng_GetRandomIntInRange
	constexpr int RESULTS_ARRAY_SIZE = 500;
	int resultsIntLessThan7[ RESULTS_ARRAY_SIZE ]	= {}; // A random roll of "4" increments count in slot[4]; slot[N-1] is # of illegal results
	int resultsIntLessThan300[ RESULTS_ARRAY_SIZE ]	= {}; // A random roll of "4" increments count in slot[4]; slot[N-1] is # of illegal results
	int resultsIntIn2Thru5[ RESULTS_ARRAY_SIZE ]	= {}; // A random roll of "4" increments count in slot[4]; slot[N-1] is # of illegal results
	int resultsIntIn6Thru8[ RESULTS_ARRAY_SIZE ]	= {}; // A random roll of "4" increments count in slot[4]; slot[N-1] is # of illegal results

	constexpr int NUM_RANDOM_ROLLS = 1'000'000;
	for( int i = 0; i < NUM_RANDOM_ROLLS; ++ i )
	{
		int randomIntLessThan7		= rng.Rng_GetRandomIntLessThan( 7 );
		int randomIntLessThan300	= rng.Rng_GetRandomIntLessThan( 300 );
		int randomIntIn2Thru5		= rng.Rng_GetRandomIntInRange( 2, 5 );
		int randomIntIn6Thru8		= rng.Rng_GetRandomIntInRange( 6, 8 );

		CountIntRngResult( randomIntLessThan7,		RESULTS_ARRAY_SIZE, resultsIntLessThan7,	0, 6 );
		CountIntRngResult( randomIntLessThan300,	RESULTS_ARRAY_SIZE, resultsIntLessThan300,	0, 299 );
		CountIntRngResult( randomIntIn2Thru5,		RESULTS_ARRAY_SIZE, resultsIntIn2Thru5,		2, 5 );
		CountIntRngResult( randomIntIn6Thru8,		RESULTS_ARRAY_SIZE, resultsIntIn6Thru8,		6, 8 );
	}

	// How many of each result do we expect, on average?
	constexpr int AVERAGE_COUNT_PER_RESULT_LESS_THAN_7		= NUM_RANDOM_ROLLS / 7;
	constexpr int AVERAGE_COUNT_PER_RESULT_LESS_THAN_300	= NUM_RANDOM_ROLLS / 300;
	constexpr int AVERAGE_COUNT_PER_RESULT_IN_2_THRU_5		= NUM_RANDOM_ROLLS / 4;
	constexpr int AVERAGE_COUNT_PER_RESULT_IN_6_THRU_8		= NUM_RANDOM_ROLLS / 3;

	// How much deviation (+/-) away from expectation will we tolerate for each result count in each category?
	constexpr int MAX_DEVIATION_COUNT_LESS_THAN_7	= AVERAGE_COUNT_PER_RESULT_LESS_THAN_7		/ 5;
	constexpr int MAX_DEVIATION_COUNT_LESS_THAN_300	= AVERAGE_COUNT_PER_RESULT_LESS_THAN_300	/ 5;
	constexpr int MAX_DEVIATION_COUNT_IN_2_THRU_5	= AVERAGE_COUNT_PER_RESULT_IN_2_THRU_5		/ 5;
	constexpr int MAX_DEVIATION_COUNT_IN_6_THRU_8	= AVERAGE_COUNT_PER_RESULT_IN_6_THRU_8		/ 5;

	VerifyAndReportIntRngTest( RESULTS_ARRAY_SIZE,	resultsIntLessThan7,	0, 6,	NUM_RANDOM_ROLLS );
	VerifyAndReportIntRngTest( RESULTS_ARRAY_SIZE,	resultsIntLessThan300,	0, 299,	NUM_RANDOM_ROLLS );
	VerifyAndReportIntRngTest( RESULTS_ARRAY_SIZE,	resultsIntIn2Thru5,		2, 5,	NUM_RANDOM_ROLLS );
	VerifyAndReportIntRngTest( RESULTS_ARRAY_SIZE,	resultsIntIn6Thru8,		6, 8,	NUM_RANDOM_ROLLS );

#endif
	return 8; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A2_MathUtils_Transforms()
{
#if defined( ENABLE_TestSet_MP1A2_MathUtils_Transforms )

	/////////////////////////////////////////
	// Tests for TransformPosition2D
	/////////////////////////////////////////
	Vector2Class origin2( 0.f, 0.f );
	Vector2Class pos2a( 0.08f, 0.06f );
	Vector2Class pos2b( -0.3f, 0.4f );
	Vector2Class pos2c( -0.8f, -0.6f );
	Vector2Class pos2d( 1.6f, -1.2f );

	// Test identity / do-nothing cases
	Vector2Class identity2a		= Function_TransformPosition2D( pos2a, 1.f, 0.f, origin2 );
	Vector2Class identity2b		= Function_TransformPosition2D( pos2b, 1.f, 360.f, origin2 );
	Vector2Class identity2c		= Function_TransformPosition2D( pos2c, 1.f, -720.f, origin2 );
	Vector2Class stillOrigin2	= Function_TransformPosition2D( origin2, 7.5f, 22.5f, origin2 );
	VerifyTestResult( IsMostlyEqual( identity2a, pos2a ),		"TransformPosition2D( scale=1, deg=0, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( identity2b, pos2b ),		"TransformPosition2D( scale=1, deg=360, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( identity2c, pos2c ),		"TransformPosition2D( scale=1, deg=-720, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( stillOrigin2, origin2 ),	"TransformPosition2D( scale=7.5, deg=22.5, trans=0,0 ) should not affect the origin(0,0)" );

	// Test 2D uniform scaling only
	Vector2Class scaled2b		= Function_TransformPosition2D( pos2b, -0.2f, 0.f, origin2 );
	Vector2Class scaled2c		= Function_TransformPosition2D( pos2c, 1.1f, 0.f, origin2 );
	VerifyTestResult( IsMostlyEqual( scaled2b, 0.06f, -0.08f ),		"TransformPosition2D( scale only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( scaled2c, -0.88f, -0.66f ),	"TransformPosition2D( scale only ) was incorrect" );

	// Test 2D rotation only
	Vector2Class rotated2b = Function_TransformPosition2D( pos2b, 1.f, 30.f, origin2 );
	Vector2Class rotated2d = Function_TransformPosition2D( pos2d, 1.f, -792.5f, origin2 );
	VerifyTestResult( IsMostlyEqual( rotated2b, -0.459808f,  0.196410f ),		"TransformPosition2D( rotation only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( rotated2d, -0.663330197f, -1.88679433f ),	"TransformPosition2D( rotation only ) was incorrect" );

	// Test 2D translation only
	Vector2Class translation2e( 101.101f, 202.202f );
	Vector2Class translation2f( -303.303f, -404.404f );
	Vector2Class translation2g( -123.456f, 789.123f );
	Vector2Class translation2h( 135.791f, -246.802f );
	Vector2Class translated2b = Function_TransformPosition2D( pos2b, 1.f, 0.f, translation2e );
	Vector2Class translated2d = Function_TransformPosition2D( pos2d, 1.f, 0.f, translation2f );
	VerifyTestResult( IsMostlyEqual( translated2b, 100.801f, 202.602f ),			"TransformPosition2D( translation only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( translated2d, -301.703003f, -405.604004f ),	"TransformPosition2D( translation only ) was incorrect" );

	// Test 2D combination transforms (scale, rotation, and/or translation)
	Vector2Class scaledRotated2b		= Function_TransformPosition2D( pos2b, 1.1f, 10.f, origin2 );
	Vector2Class scaledRotated2d		= Function_TransformPosition2D( pos2d, -11.f, -170.f, origin2 );
	Vector2Class scaledTranslated2b		= Function_TransformPosition2D( pos2b, 2.2f, 0.f, translation2g );
	Vector2Class scaledTranslated2d		= Function_TransformPosition2D( pos2d, -0.2f, 0.f, translation2h );
	Vector2Class rotatedTranslated2b	= Function_TransformPosition2D( pos2b, 1.f, -15.f, translation2f );
	Vector2Class rotatedTranslated2d	= Function_TransformPosition2D( pos2d, 1.f, 909.9f, translation2e );
	Vector2Class fullyTransformed2b		= Function_TransformPosition2D( pos2b, 2.f, 90.f, translation2h );
	Vector2Class fullyTransformed2d		= Function_TransformPosition2D( pos2d, -0.1f, -530.345f, translation2g );
	VerifyTestResult( IsMostlyEqual( scaledRotated2b, -0.401391804f, 0.376011491f ),	"TransformPosition2D( scale, rotate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( scaledRotated2d, 19.6247711f, -9.94325638f ),		"TransformPosition2D( scale, rotate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( scaledTranslated2b, -124.116005f, 790.002991f ),	"TransformPosition2D( scale, translate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( scaledTranslated2d, 135.470993f, -246.561996f ),	"TransformPosition2D( scale, translate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( rotatedTranslated2b, -303.489258f, -403.939972f ),	"TransformPosition2D( rotate, translate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( rotatedTranslated2d, 99.3185043f, 203.109039f ),	"TransformPosition2D( rotate, translate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( fullyTransformed2b, 134.990997f, -247.402008f ),	"TransformPosition2D( scale, rotate, translate ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( fullyTransformed2d, -123.278141f, 789.031534f ),	"TransformPosition2D( scale, rotate, translate ) was incorrect" );

	/////////////////////////////////////////
	// Tests for TransformPosition3DXY
	/////////////////////////////////////////
	Vector3Class origin3( 0.f, 0.f, 0.f );
	Vector3Class pos3a( 0.08f, 0.06f, 0.f );
	Vector3Class pos3b( -0.3f, 0.4f, 1.1f );
	Vector3Class pos3c( -0.8f, -0.6f, -0.5f );
	Vector3Class pos3d( 1.6f, -1.2f, -0.1f );

	// Test identity / do-nothing cases
	Vector3Class identity3a		= Function_TransformPosition3DXY( pos3a, 1.f, 0.f, origin2 );
	Vector3Class identity3b		= Function_TransformPosition3DXY( pos3b, 1.f, 360.f, origin2 );
	Vector3Class identity3c		= Function_TransformPosition3DXY( pos3c, 1.f, -720.f, origin2 );
	Vector3Class stillOrigin3	= Function_TransformPosition3DXY( origin3, 7.5f, 22.5f, origin2 );
	VerifyTestResult( IsMostlyEqual( identity3a, pos3a ),		"TransformPosition3DXY( scale=1, deg=0, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( identity3b, pos3b ),		"TransformPosition3DXY( scale=1, deg=360, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( identity3c, pos3c ),		"TransformPosition3DXY( scale=1, deg=-720, trans=0,0 ) should not move the position" );
	VerifyTestResult( IsMostlyEqual( stillOrigin3, origin3 ),	"TransformPosition3DXY( scaleXY, rotateXY ) should not affect the origin(0,0,0)" );

	// Test scaleXY only
	Vector3Class scaled3b = Function_TransformPosition3DXY( pos3b, -0.2f, 0.f, origin2 );
	Vector3Class scaled3c = Function_TransformPosition3DXY( pos3c, 1.1f, 0.f, origin2 );
	VerifyTestResult( IsMostlyEqual( scaled3b, 0.06f, -0.08f, 1.1f ),	"TransformPosition3DXY( scaleXY only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( scaled3c, -0.88f, -0.66f, -0.5f ), "TransformPosition3DXY( scaleXY only ) was incorrect" );

	// Test XY rotation only
	Vector3Class rotated3b = Function_TransformPosition3DXY( pos3b, 1.f, 30.f, origin2 );
	Vector3Class rotated3d = Function_TransformPosition3DXY( pos3d, 1.f, -792.5f, origin2 );
	VerifyTestResult( IsMostlyEqual( rotated3b, -0.459808f,  0.196410f, 1.1f ),			"TransformPosition3DXY( rotateXY only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( rotated3d, -0.663330197f, -1.88679433f, -0.1f ),	"TransformPosition3DXY( rotateXY only ) was incorrect" );

	// Test XY translation only
	Vector3Class translated3b = Function_TransformPosition3DXY( pos3b, 1.f, 0.f, translation2e );
	Vector3Class translated3d = Function_TransformPosition3DXY( pos3d, 1.f, 0.f, translation2f );
	VerifyTestResult( IsMostlyEqual( translated3b, 100.801f, 202.602f, 1.1f ),			"TransformPosition3DXY( translateXY only ) was incorrect" );
	VerifyTestResult( IsMostlyEqual( translated3d, -301.703003f, -405.604004f, -0.1f ),	"TransformPosition3DXY( translateXY only ) was incorrect" );

	// Test combination transforms (scale, rotation, and/or translation: SR, ST, RT, SRT)
	Vector3Class scaledRotated3b		= Function_TransformPosition3DXY( pos3b, 1.1f, 10.f, origin2 );
	Vector3Class scaledRotated3d		= Function_TransformPosition3DXY( pos3d, -11.f, -170.f, origin2 );
	Vector3Class scaledTranslated3b		= Function_TransformPosition3DXY( pos3b, 2.2f, 0.f, translation2g );
	Vector3Class scaledTranslated3d		= Function_TransformPosition3DXY( pos3d, -0.2f, 0.f, translation2h );
	Vector3Class rotatedTranslated3b	= Function_TransformPosition3DXY( pos3b, 1.f, -15.f, translation2f );
	Vector3Class rotatedTranslated3d	= Function_TransformPosition3DXY( pos3d, 1.f, 909.9f, translation2e );
	Vector3Class fullyTransformed3b		= Function_TransformPosition3DXY( pos3b, 2.f, 90.f, translation2h );
	Vector3Class fullyTransformed3d		= Function_TransformPosition3DXY( pos3d, -0.1f, -530.345f, translation2g );

	bool isScaledRotated3bCorrect		= IsMostlyEqual( scaledRotated3b, -0.401391804f, 0.376011491f, 1.1f );
	bool isScaledRotated3dCorrect		= IsMostlyEqual( scaledRotated3d, 19.6247711f, -9.94325638f, -0.1f );
	bool isScaledTranslated3bCorrect	= IsMostlyEqual( scaledTranslated3b, -124.116005f, 790.002991f, 1.1f );
	bool isScaledTranslated3dCorrect	= IsMostlyEqual( scaledTranslated3d, 135.470993f, -246.561996f, -0.1f );
	bool isRotatedTranslated3bCorrect	= IsMostlyEqual( rotatedTranslated3b, -303.489258f, -403.939972f, 1.1f );
	bool isRotatedTranslated3dCorrect	= IsMostlyEqual( rotatedTranslated3d, 99.3185043f, 203.109039f, -0.1f );
	bool isFullyTransformed3bCorrect	= IsMostlyEqual( fullyTransformed3b, 134.990997f, -247.402008f, 1.1f  );
	bool isFullyTransformed3dCorrect	= IsMostlyEqual( fullyTransformed3d, -123.278141f, 789.031534f, -0.1f );

	VerifyTestResult( isScaledRotated3bCorrect,		"TransformPosition3DXY( scaleXY, rotateXY ) was incorrect" );
	VerifyTestResult( isScaledRotated3dCorrect,		"TransformPosition3DXY( scaleXY, rotateXY ) was incorrect" );
	VerifyTestResult( isScaledTranslated3bCorrect,	"TransformPosition3DXY( scaleXY, translateXY ) was incorrect" );
	VerifyTestResult( isScaledTranslated3dCorrect,	"TransformPosition3DXY( scaleXY, translateXY ) was incorrect" );
	VerifyTestResult( isRotatedTranslated3bCorrect,	"TransformPosition3DXY( rotateXY, translateXY ) was incorrect" );
	VerifyTestResult( isRotatedTranslated3dCorrect,	"TransformPosition3DXY( rotateXY, translateXY ) was incorrect" );
	VerifyTestResult( isFullyTransformed3bCorrect,	"TransformPosition3DXY( scaleXY, rotateXY, translateXY ) was incorrect" );
	VerifyTestResult( isFullyTransformed3dCorrect,	"TransformPosition3DXY( scaleXY, rotateXY, translateXY ) was incorrect" );

#endif
	return 36; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A2()
{
	RunTestSet( true, TestSet_MP1A2_MathUtils_Angles,		"MP1-A2: MathUtils angles" );
	RunTestSet( true, TestSet_MP1A2_MathUtils_Queries,		"MP1-A2: MathUtils queries" );
	RunTestSet( true, TestSet_MP1A2_Vec2_Methods,			"MP1-A2: Vec2 methods" );
	RunTestSet( true, TestSet_MP1A2_Vec3_Basics,			"MP1-A2: Vec3 basics" );
	RunTestSet( true, TestSet_MP1A2_Vec3_Methods,			"MP1-A2: Vec3 methods" );
	RunTestSet( true, TestSet_MP1A2_RandomNumberGenerator,	"MP1-A2: RandomNumberGenerator" );
	RunTestSet( true, TestSet_MP1A2_MathUtils_Transforms,	"MP1-A2: MathUtils transforms" );
}

