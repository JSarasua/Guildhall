//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A1.cpp
//
#include "Game/UnitTests_MP1A1.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		DO NOT MODIFY ANY CODE BELOW HERE WITHOUT EXPRESS PERMISSION FROM YOUR PROFESSOR
//		(as doing so will be considered cheating and have serious academic consequences)
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Simple "close enough" compares for ints, to test actual vs. expected outcomes on large int ops
// Note: mostly used for checking random number statistical results
//
bool IsMostlyEqual( int a, int b, int epsilon=0 )
{
	int diff = a - b;
	if( diff <= epsilon && diff >= -epsilon )
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
// Simple "close enough" compares for floats, to test actual vs. expected outcomes on float ops
//
bool IsMostlyEqual( float a, float b, float epsilon=0.001f )
{
	float diff = a - b;
	if( diff <= epsilon && diff >= -epsilon )
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector2Class& vec2, float x, float y )
{
	return IsMostlyEqual( vec2.x, x ) && IsMostlyEqual( vec2.y, y );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector2Class& vec2a, const Vector2Class& vec2b )
{
	return IsMostlyEqual( vec2a.x, vec2b.x ) && IsMostlyEqual( vec2a.y, vec2b.y );
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A1_Vec2Construction()
{
#if defined( ENABLE_TestSet_MP1A1_Vec2Construction )

	Vector2Class v1;								// default constructor
	Vector2Class v2( 1.4f, -1.6f );					// explicit constructor (x,y)
	Vector2Class v3 = Vector2Class( -1.3f, 1.7f );	// explicit constructor (per C++ standard)
	Vector2Class v4( v2 );							// copy constructor
	Vector2Class v5 = v3;							// copy constructor (per C++ standard)

	VerifyTestResult( sizeof( Vector2Class ) == 8, "sizeof(Vec2) was not 8 bytes" );
	VerifyTestResult( v1.x == 0.f && v1.y == 0.f, "Vec2() default constructor failed to initialize x,y to (0,0)" );
	VerifyTestResult( v2.x ==  1.4f, "Vec2( x, y ) : explicit constructor failed to assign x" );
	VerifyTestResult( v2.y == -1.6f, "Vec2( x, y ) : explicit constructor failed to assign y" );
	VerifyTestResult( v4.x ==  1.4f, "Vec2( vec2 ) : copy constructor failed to copy x" );
	VerifyTestResult( v4.y == -1.6f, "Vec2( vec2 ) : copy constructor failed to copy y" );

#endif
	return 6; // Number of tests expected from this test set
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A1_Vec2TemporaryOperators()
{
#if defined( ENABLE_TestSet_MP1A1_Vec2TemporaryOperators )

	Vector2Class v1( 3.5f, -1.5f );					// explicit constructor from x,y
	Vector2Class v2 = Vector2Class( -0.5f, 1.0f );	// explicit constructor from x,y

	Vector2Class v3 = v1 + v2;						// operator+ (vec2 + vec2)
	Vector2Class v4 = v1 + v1;						// operator+ (vec2 + vec2), added with itself
	Vector2Class v5 = v1 - v2;						// operator- (vec2 - vec2)
	Vector2Class v6 = v1 - v1;						// operator- (vec2 - vec2), subtract from itself
	Vector2Class v7 = v1 * 2.0f;					// operator* (vec2 * float)
	Vector2Class v8 = 2.0f * v1;					// operator* (float * vec2)
	Vector2Class v9 = v1 / 2.0f;					// operator/ (vec2 / float)

	VerifyTestResult( v3.x == 3.0f,		"Vec2::operator+ : did not add x in (vec2 + vec2) correctly" );
	VerifyTestResult( v4.x == 7.0f,		"Vec2::operator+ : did not add x in (vec2 + vec2) with itself correctly" );
	VerifyTestResult( v5.x == 4.0f,		"Vec2::operator- : did not subtract x in (vec2 - vec2) correctly" );
	VerifyTestResult( v6.x == 0.0f,		"Vec2::operator- : did not subtract x in (vec2 - vec2) from itself correctly" );
	VerifyTestResult( v7.x == 7.0f,		"Vec2::operator* : did not scale x in (vec2 * float) correctly" );
	VerifyTestResult( v8.x == 7.0f,		"Vec2::operator* : did not scale x in (float * vec2) correctly" );
	VerifyTestResult( v9.x == 1.75f, 	"Vec2::operator/ : did not divide x in (vec2 / float) correctly" );

	VerifyTestResult( v3.y == -0.5f,	"Vec2::operator+ : did not add y in (vec2 + vec2) correctly" );
	VerifyTestResult( v4.y == -3.0f,	"Vec2::operator+ : did not add y in (vec2 + vec2) with itself correctly" );
	VerifyTestResult( v5.y == -2.5f,	"Vec2::operator- : did not subtract y in (vec2 - vec2) correctly" );
	VerifyTestResult( v6.y ==  0.0f,	"Vec2::operator- : did not subtract y in (vec2 - vec2) from itself correctly" );
	VerifyTestResult( v7.y == -3.0f,	"Vec2::operator* : did not scale y in (vec2 * float) correctly" );
	VerifyTestResult( v8.y == -3.0f,	"Vec2::operator* : did not scale y in (float * vec2) correctly" );
	VerifyTestResult( v9.y == -0.75f,	"Vec2::operator/ : did not divide y in (vec2 / float) correctly" );

#endif
	return 14; // Number of tests expected from this test set
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A1_Vec2ComparisonOperators()
{
#if defined( ENABLE_TestSet_MP1A1_Vec2ComparisonOperators )

	Vector2Class v1( 3.f, 4.f );					// explicit constructor
	Vector2Class v2( 3.f, 4.f );					// explicit constructor
	Vector2Class v3( 3.f, 99.f );					// explicit constructor
	Vector2Class v4( 99.f, 4.f );					// explicit constructor
	Vector2Class v5( 3.f, 3.f );					// explicit constructor
	Vector2Class v6( 4.f, 4.f );					// explicit constructor
	Vector2Class v7( 4.f, 3.f );					// explicit constructor

	VerifyTestResult( v1 == v2,		"Vec2::operator== : failed to detect match" );
	VerifyTestResult( !(v1 != v2),	"Vec2::operator!= : failed to reject match" );

	VerifyTestResult( v1 != v3,		"Vec2::operator!= : failed to detect y-mismatch" );
	VerifyTestResult( v1 != v4,		"Vec2::operator!= : failed to detect x-mismatch" );
	VerifyTestResult( v1 != v5,		"Vec2::operator!= : failed to detect y-mismatch coincidence" );
	VerifyTestResult( v1 != v6,		"Vec2::operator!= : failed to detect x-mismatch coincidence" );
	VerifyTestResult( v1 != v7,		"Vec2::operator!= : failed to detect x/y mixup" );

	VerifyTestResult( !(v1 == v3),	"Vec2::operator== : failed to reject y-mismatch" );
	VerifyTestResult( !(v1 == v4),	"Vec2::operator== : failed to reject x-mismatch" );
	VerifyTestResult( !(v1 == v5),	"Vec2::operator== : failed to reject y-mismatch coincidence" );
	VerifyTestResult( !(v1 == v6),	"Vec2::operator== : failed to reject x-mismatch coincidence" );
	VerifyTestResult( !(v1 == v7),	"Vec2::operator== : failed to reject x/y mixup" );

#endif	
	return 12; // Number of tests expected from this test set
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A1_Vec2AssignmentOperators()
{
#if defined( ENABLE_TestSet_MP1A1_Vec2AssignmentOperators )

	Vector2Class v1( 3.f, 4.f );					// explicit constructor
	Vector2Class v2;								// default constructor
	v2 = v1;										// operator= (assignment operator)
	v1 += Vector2Class( 1.f, 2.f );					// operator+= (add-and-assign operator)
	Vector2Class v3( 3.f, 4.f );					// 
	v3 -= Vector2Class( 2.f, 1.f );					// operator-= (subtract-and-assign operator)
	Vector2Class v4( 3.f, 4.f );					// 
	Vector2Class v5( 5.f, 7.f );					// 
	Vector2Class v6( 1.f, 2.f );					// 
	Vector2Class v7( 8.f, 6.f );					// 
	v4 += v5;										// 
	v5 -= v6;										// 
	v6 *= 3.f;										// operator*= (uniform scale by float)
	v7 /= 2.f;										// operator/= (uniform scale by 1/float)

	VerifyTestResult( v2.x == 3.f,	"Vec2::operator= : failed to copy x" );
	VerifyTestResult( v2.y == 4.f,	"Vec2::operator= : failed to copy y" );
	VerifyTestResult( v1.x == 4.f,	"Vec2::operator+= : failed to add x" );
	VerifyTestResult( v1.y == 6.f,	"Vec2::operator+= : failed to add y" );
	VerifyTestResult( v3.x == 1.f,	"Vec2::operator-= : failed to subtract x" );
	VerifyTestResult( v3.y == 3.f,	"Vec2::operator-= : failed to subtract y" );
	VerifyTestResult( v4.x == 8.f,	"Vec2::operator+= : failed to add existing vec2's x" );
	VerifyTestResult( v4.y == 11.f,	"Vec2::operator+= : failed to add existing vec2's y" );
	VerifyTestResult( v5.x == 4.f,	"Vec2::operator-= : failed to subtract existing vec2's x" );
	VerifyTestResult( v5.y == 5.f,	"Vec2::operator-= : failed to subtract existing vec2's y" );
	VerifyTestResult( v6.x == 3.f,	"Vec2::operator*= : failed to scale x" );
	VerifyTestResult( v6.y == 6.f,	"Vec2::operator*= : failed to scale y" );
	VerifyTestResult( v7.x == 4.f,	"Vec2::operator/= : failed to divide x" );
	VerifyTestResult( v7.y == 3.f,	"Vec2::operator/= : failed to divide y" );

#endif	
	return 14; // Number of tests expected from this test set
}


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A1()
{
	RunTestSet( true, TestSet_MP1A1_Vec2Construction,			"MP1-A1: Vec2 construction" );
	RunTestSet( true, TestSet_MP1A1_Vec2TemporaryOperators,		"MP1-A1: Vec2 temp operators" );
	RunTestSet( true, TestSet_MP1A1_Vec2ComparisonOperators,	"MP1-A1: Vec2 comparison operators" );
	RunTestSet( true, TestSet_MP1A1_Vec2AssignmentOperators,	"MP1-A1: Vec2 assignment operators" );
}

