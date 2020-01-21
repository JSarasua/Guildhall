//-----------------------------------------------------------------------------------------------
// UnitTests_Custom.cpp
//
// YOU MAY CHANGE anything in this file.  Make sure your "RunTestSet()" commands at the bottom
//	of the file in RunTest_Custom() all pass "false" for their first argument (isGraded).
//
#include "Game/UnitTests_Custom.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
// Include whatever you want here to test various engine classes (even non-math stuff, like Rgba)


//-----------------------------------------------------------------------------------------------
int TestSet_Custom_Dummy()
{
	VerifyTestResult( 2 == 2, "Dummy example test that passes (2 == 2)" ); // e.g. a non-graded test that passes
	VerifyTestResult( 2 == 3, "Dummy example test that fails (2 == 3)" ); // e.g. a non-graded test that doesn't pass

	return 2; // Number of tests expected (equal to the # of times you call VerifyTestResult)
}


//-----------------------------------------------------------------------------------------------
int TestSet_Custom_Rgba()
{
	// Write your own tests here; each call to VerifyTestResult is considered a "test".

	return 0; // Number of tests expected (set equal to the # of times you call VerifyTestResult)
}

int TestSet_Custom_MathUtils()
{
	AABB2 aabb0to10 =	AABB2( Vec2( 0.f, 0.f ), Vec2( 10.f, 10.f ) );
	AABB2 aabb5to15 =	AABB2( Vec2( 5.f, 5.f ), Vec2( 15.f, 15.f ) );
	AABB2 aabb20to25 =	AABB2( Vec2(20.f,20.f), Vec2( 25.f, 25.f ) );
	VerifyTestResult(DoAABBsOverlap2D(aabb0to10,aabb5to15), "DoAABBsOverlap2D 0 to 10, 5 to 15");
	VerifyTestResult(DoAABBsOverlap2D(aabb0to10,aabb0to10), "DoAABBsOverlap2D SAME");
	VerifyTestResult(!DoAABBsOverlap2D(aabb0to10, aabb20to25), "DoAABBsOverlap2D 0 to 10, 20 to 25");

	return 3;
}

int TestSet_Custom_SetFromText()
{
	Vec2 vec2D15(1.f,5.f);
	IntVec2 intVec2D15(1,5);
	Rgba8 rgba810203040(10,20,30,40);
	Rgba8 rgba8102030(10,20,30,255);

	Vec2 defaultVec2;
	IntVec2 defaultIntVec2;
	Rgba8 defaultRgba8;
	Rgba8 defaultRgb8;

	defaultVec2.SetFromText("1,5");
	defaultIntVec2.SetFromText("1,5");
	defaultRgba8.SetFromText("10,20,30,40");
	defaultRgb8.SetFromText("10,20,30");



	VerifyTestResult( vec2D15 == defaultVec2, "Vec2 is not 1,5");
	VerifyTestResult( intVec2D15 == defaultIntVec2, "IntVec2 is not 1,5");
	VerifyTestResult( rgba810203040 == defaultRgba8, "Rgba8 with alpha is not 10,20,30,40");
	VerifyTestResult( rgba8102030 == defaultRgb8, "Rgba8 with alpha is not 10,20,30,255");


	return 4;
}

int TestSet_Custom_OBB2()
{
	AABB2 abb0(1.f,1.f,3.f,3.f);
	AABB2 abb1(1.f,1.f,6.f,6.f);
	OBB2 obb0 =	OBB2(abb0,0.f);
	OBB2 obb0Rotated30Degrees = OBB2(abb0,30.f);
	OBB2 obb1Rotated30Degrees = OBB2(abb1,30.f);

	Vec2 pointAt11 = Vec2(3.f,3.f);
	Vec2 calculatedPointAt11 = obb0.GetPointAtUV(Vec2(1.f,1.f));

	Vec2 pointAt00 = Vec2( 1.f, 1.f );
	Vec2 calculatedPointAt00 = obb0.GetPointAtUV( Vec2( 0.f, 0.f ) );

	Vec2 pointAt0505 = Vec2( 2.f, 2.f );
	Vec2 calculatedPointAt0505 = obb0.GetPointAtUV( Vec2( 0.5f, 0.5f ) );


	Vec2 pointAt10Rotated30Degrees = TransformPosition2D(Vec2(1.f,0.f),1.f,30.f,Vec2(2.f,2.f));
	Vec2 calculatedPointAt10Rotated30Degrees = obb0Rotated30Degrees.GetPointAtUV(Vec2(1.f,0.5f));
	Vec2 calculatedUVsAt30Degreess = obb0Rotated30Degrees.GetUVForPoint(pointAt10Rotated30Degrees);

	Vec2 pointAt10Rotated30DegreesOBB1 = TransformPosition2D(Vec2(2.5f,0.f),1.f,30.f,Vec2(3.5f,3.5f));
	Vec2 calculatedPointAt10Rotated30DegreesOBB1 = obb1Rotated30Degrees.GetPointAtUV( Vec2( 1.f, 0.5f ) );
	Vec2 calculatedUVsAt30DegreessOBB1 = obb1Rotated30Degrees.GetUVForPoint( pointAt10Rotated30DegreesOBB1 );

	VerifyTestResult( IsMostlyEqual(pointAt11, calculatedPointAt11 ), "Failed for UV 1,1: expected 3,3");
	VerifyTestResult( IsMostlyEqual(pointAt00, calculatedPointAt00 ), "Failed for UV 0,0: expected 1,1");
	VerifyTestResult( IsMostlyEqual(pointAt0505, calculatedPointAt0505 ), "Failed for UV 0.5,0.5: expected 2,2");

	VerifyTestResult( IsMostlyEqual(pointAt10Rotated30Degrees, calculatedPointAt10Rotated30Degrees ), "Failed for UV 1,0 with OBB2 rotated 30 degrees");
	VerifyTestResult( IsMostlyEqual(calculatedUVsAt30Degreess, Vec2(1.f,0.5f)), "Failed GetUVForPoint rotated 30 degrees");

	VerifyTestResult( IsMostlyEqual( pointAt10Rotated30DegreesOBB1, calculatedPointAt10Rotated30DegreesOBB1 ), "Failed for UV 1,0 with OBB2 rotated 30 degrees" );
	VerifyTestResult( IsMostlyEqual( calculatedUVsAt30DegreessOBB1, Vec2( 1.f, 0.5f ) ), "Failed GetUVForPoint rotated 30 degrees" );



	return 7;
}

int TestSet_Custom_AABB_Carve_Tests()
{
	AABB2 aabb34To75(Vec2(3,4), Vec2(7,5));
	AABB2 carvedBox = aabb34To75.CarveBoxOffLeft(0.25f);

	VerifyTestResult( aabb34To75.mins.x == 4. , "Testing carve tests old box" );
	VerifyTestResult( carvedBox.maxs.x == 4. , "Testing carve tests new box" );

	return 2;
}


//-----------------------------------------------------------------------------------------------
void RunTests_Custom()
{
	// Always set first argument to "false" for Custom tests, so they don't interfere with grading
	RunTestSet( false, TestSet_Custom_Dummy,			"Custom dummy sample tests" );
	RunTestSet( false, TestSet_Custom_Rgba,				"Custom Rgba sample tests" );
	RunTestSet( false, TestSet_Custom_MathUtils,		"Custom AABB2 Test" );
	RunTestSet( false, TestSet_Custom_SetFromText,		"Custom SetFromText tests" );
	RunTestSet( false, TestSet_Custom_OBB2,				"Custom OBB2 tests" );
	RunTestSet( false, TestSet_Custom_AABB_Carve_Tests,	"Custom AABB2 carve tests" );
}

