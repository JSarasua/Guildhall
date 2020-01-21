//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A5.cpp
//
#include "Game/UnitTests_MP1A5.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h> // for memcpy


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
bool IsMostlyEqual( const AABB2Class& box1, const AABB2Class& box2 );
bool IsMostlyEqual( const AABB2Class& box, const Vector2Class& mins, const Vector2Class& maxs );
bool IsMostlyEqual( const AABB2Class& box, float minX, float minY, float maxX, float maxY );
bool IsEqual( const IntVec2Class& a, const IntVec2Class& b );
bool IsEqual( const IntVec2Class& ivec2, int x, int y );

//------------------------------------------------------------------------------------------------
// New helper functions introduced here
//


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector4Class& vec4, float x, float y, float z, float w )
{
	return IsMostlyEqual( vec4.x, x ) && IsMostlyEqual( vec4.y, y ) && IsMostlyEqual( vec4.z, z ) && IsMostlyEqual( vec4.w, w );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Vector4Class& vec4a, const Vector4Class& vec4b )
{
	return IsMostlyEqual( vec4a, vec4b.x, vec4b.y, vec4b.z, vec4b.w );
}


//------------------------------------------------------------------------------------------------
bool IsMostlyEqual( const float* floatArray1, const float* floatArray2, int numArrayElements )
{
	for( int i = 0; i < numArrayElements; ++ i )
	{
		float value1 = floatArray1[ i ];
		float value2 = floatArray2[ i ];
		if( !IsMostlyEqual( value1, value2 ) )
			return false;
	}

	return true;
}


//------------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Matrix44Class& matrix, const float* sixteenCorrectMatrixValues )
{
	bool isCorrectSize = sizeof(Matrix44Class) == 64;
	if( !isCorrectSize )
		return false;

	float* matrixAsFloatArray = (float*) &matrix;
	return IsMostlyEqual( matrixAsFloatArray, sixteenCorrectMatrixValues, 16 );
}


//-----------------------------------------------------------------------------------------------
bool IsMostlyEqual( const Matrix44Class& mat44a, const Matrix44Class& mat44b )
{
	return IsMostlyEqual( mat44a, (float*) &mat44b );
}





//////////////////////////////////////////////////////////////////////////////////////////////////
// Tests follow
//////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixConstruction()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixConstruction )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float identityValues[16]		= { 1.f, 0.f, 0.f, 0.f,   0.f, 1.f, 0.f, 0.f,   0.f, 0.f, 1.f, 0.f,     0.f, 0.f, 0.f, 1.f };
	float explicit2DValues[16]		= { 1.f, 2.f, 0.f, 0.f,   3.f, 4.f, 0.f, 0.f,   0.f, 0.f, 1.f, 0.f,     5.f, 6.f, 0.f, 1.f };
	float explicit3DValues[16]		= { 1.f, 2.f, 3.f, 0.f,   4.f, 5.f, 6.f, 0.f,   7.f, 8.f, 9.f, 0.f,     10.f, 11.f, 12.f, 1.f };
	float explicit4DValues[16]		= { 1.f, 2.f, 3.f, 4.f,   5.f, 6.f, 7.f, 8.f,   9.f, 10.f, 11.f, 12.f,  13.f, 14.f, 15.f, 16.f };

	Vector2Class iBasis2D( 1.f, 2.f );
	Vector2Class jBasis2D( 3.f, 4.f );
	Vector2Class translation2D( 5.f, 6.f );

	Vector3Class iBasis3D( 1.f, 2.f, 3.f );
	Vector3Class jBasis3D( 4.f, 5.f, 6.f );
	Vector3Class kBasis3D( 7.f, 8.f, 9.f );
	Vector3Class translation3D( 10.f, 11.f, 12.f );

	Vector4Class iBasis4D( 1.f, 2.f, 3.f, 4.f );
	Vector4Class jBasis4D( 5.f, 6.f, 7.f, 8.f );
	Vector4Class kBasis4D( 9.f, 10.f, 11.f, 12.f );
	Vector4Class translation4D( 13.f, 14.f, 15.f, 16.f );

	VerifyTestResult( sizeof(Matrix44Class) == 64, "sizeof(Matrix) was wrong, should be 64 bytes (16 packed floats)" );

	Matrix44Class identity;
	VerifyTestResult( IsMostlyEqual( identity, identityValues ), "Matrix should default-construct to Identity" );

	Matrix44Class explicit2D( iBasis2D, jBasis2D, translation2D );
	Matrix44Class explicit3D( iBasis3D, jBasis3D, kBasis3D, translation3D );
	Matrix44Class explicit4D( iBasis4D, jBasis4D, kBasis4D, translation4D );
	VerifyTestResult( IsMostlyEqual( explicit2D, explicit2DValues ), "Matrix explicit constructor from (iBasis2D, jBasis2D, translation2D) was incorrect" );
	VerifyTestResult( IsMostlyEqual( explicit3D, explicit3DValues ), "Matrix explicit constructor from (iBasis3D, jBasis3D, kBasis3D, translation3D) was incorrect" );
	VerifyTestResult( IsMostlyEqual( explicit4D, explicit4DValues ), "Matrix explicit constructor from (iBasis4D, jBasis4D, kBasis4D, translation4D) was incorrect" );

	Matrix44Class explicitFloat16Array( explicit4DValues );
	VerifyTestResult( IsMostlyEqual( explicitFloat16Array, explicit4DValues ), "Matrix explicit constructor from float* (float[16]) was incorrect" );

#endif
	return 6; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixStaticCreates()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixStaticCreates )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float identityValues[16]			= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float translation2DValues[16]		= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   5.f, 9.f, 0.f, 1.f };
	float translation3DValues[16]		= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   6.f, 7.f, 8.f, 1.f };
	float uniformScale2DValues[16]		= { 7.f, 0.f, 0.f, 0.f,		0.f, 7.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float uniformScale3DValues[16]		= { 8.f, 0.f, 0.f, 0.f,		0.f, 8.f, 0.f, 0.f,		0.f, 0.f, 8.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float nonUniformScale2DValues[16]	= { 3.f, 0.f, 0.f, 0.f,		0.f, 4.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float nonUniformScale3DValues[16]	= { 7.f, 0.f, 0.f, 0.f,		0.f, 8.f, 0.f, 0.f,		0.f, 0.f, 9.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float zRotationValues[16]			= { .8f, .6f, 0.f, 0.f,		-.6f, .8f, 0.f, 0.f,	0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float yRotationValues[16]			= { .8f, 0.f, -.6f, 0.f,	0.f, 1.f, 0.f, 0.f,		.6f, 0.f, .8f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float xRotationValues[16]			= { 1.f, 0.f, 0.f, 0.f,		0.f, .8f, .6f, 0.f,		0.f, -.6f, .8f, 0.f,   0.f, 0.f, 0.f, 1.f };

	Matrix44Class noTranslation2D = Matrix44Class::Mat44_CreateTranslationXY( Vector2Class( 0.f, 0.f ) );
	Matrix44Class translation2D = Matrix44Class::Mat44_CreateTranslationXY( Vector2Class( 5.f, 9.f ) );
	VerifyTestResult( IsMostlyEqual( noTranslation2D, identityValues ), "Translation matrix should be identity for translation=(0,0)" );
	VerifyTestResult( IsMostlyEqual( translation2D, translation2DValues ), "Translation matrix was incorrect for translation=(5,9)" );

	Matrix44Class noTranslation3D = Matrix44Class::Mat44_CreateTranslation3D( Vector3Class( 0.f, 0.f, 0.f ) );
	Matrix44Class translation3D = Matrix44Class::Mat44_CreateTranslation3D( Vector3Class( 6.f, 7.f, 8.f ) );
	VerifyTestResult( IsMostlyEqual( noTranslation3D, identityValues ), "Translation matrix should be identity for translation=(0,0,0)" );
	VerifyTestResult( IsMostlyEqual( translation3D, translation3DValues ), "Translation matrix was incorrect for translation=(6,7,8)" );

	Matrix44Class noUniformScale2D	= Matrix44Class::Mat44_CreateUniformScaleXY( 1.f );
	Matrix44Class uniformScale2D	= Matrix44Class::Mat44_CreateUniformScaleXY( 7.f );
	VerifyTestResult( IsMostlyEqual( noUniformScale2D, identityValues ), "UniformScale2D matrix should be identity for scale=1.0" );
	VerifyTestResult( IsMostlyEqual( uniformScale2D, uniformScale2DValues ), "UniformScale2D matrix incorrect for scale=7.0 (note: scaleZ should be 1.0)" );

	Matrix44Class noNonUniformScale2D	= Matrix44Class::Mat44_CreateNonUniformScaleXY( Vector2Class( 1.f, 1.f ) );
	Matrix44Class nonUniformScale2D		= Matrix44Class::Mat44_CreateNonUniformScaleXY( Vector2Class( 3.f, 4.f ) );
	VerifyTestResult( IsMostlyEqual( noNonUniformScale2D, identityValues ), "NonUniformScale2D matrix should be identity for scale=1" );
	VerifyTestResult( IsMostlyEqual( nonUniformScale2D, nonUniformScale2DValues ), "NonUniformScale2D matrix incorrect for scale=(3,4) (note: scaleZ should be 1.0)" );

	Matrix44Class noUniformScale3D	= Matrix44Class::Mat44_CreateUniformScale3D( 1.f );
	Matrix44Class uniformScale3D	= Matrix44Class::Mat44_CreateUniformScale3D( 8.f );
	VerifyTestResult( IsMostlyEqual( noUniformScale3D, identityValues ), "UniformScale3D matrix should be identity for scale=1.0" );
	VerifyTestResult( IsMostlyEqual( uniformScale3D, uniformScale3DValues ), "UniformScale3D matrix incorrect for scale=8.0" );

	Matrix44Class noNonUniformScale3D	= Matrix44Class::Mat44_CreateNonUniformScale3D( Vector3Class( 1.f, 1.f, 1.f ) );
	Matrix44Class nonUniformScale3D		= Matrix44Class::Mat44_CreateNonUniformScale3D( Vector3Class( 7.f, 8.f, 9.f ) );
	VerifyTestResult( IsMostlyEqual( noNonUniformScale3D, identityValues ), "NonUniformScale3D matrix should be identity for scale=1" );
	VerifyTestResult( IsMostlyEqual( nonUniformScale3D, nonUniformScale3DValues ), "NonUniformScale3D matrix incorrect for scale=(7,8,9)" );

	Matrix44Class noZRotation = Matrix44Class::Mat44_CreateZRotationDegrees( 0.f );
	Matrix44Class zRotation = Matrix44Class::Mat44_CreateZRotationDegrees( 36.8698976f );
	VerifyTestResult( IsMostlyEqual( noZRotation, identityValues ), "ZRotationDegrees matrix should be identity for rotationDegrees=0.0" );
	VerifyTestResult( IsMostlyEqual( zRotation, zRotationValues ), "ZRotationDegrees matrix incorrect for 36.8698976 degrees - iBasis should be (.8,.6,.0), etc." );

	Matrix44Class noYRotation = Matrix44Class::Mat44_CreateYRotationDegrees( 0.f );
	Matrix44Class yRotation = Matrix44Class::Mat44_CreateYRotationDegrees( 36.8698976f );
	VerifyTestResult( IsMostlyEqual( noYRotation, identityValues ), "YRotationDegrees matrix should be identity for rotationDegrees=0.0" );
	VerifyTestResult( IsMostlyEqual( yRotation, yRotationValues ), "YRotationDegrees matrix incorrect for 36.8698976 degrees - iBasis should be (.8,0,-.6), etc." );

	Matrix44Class noXRotation = Matrix44Class::Mat44_CreateXRotationDegrees( 0.f );
	Matrix44Class xRotation = Matrix44Class::Mat44_CreateXRotationDegrees( 36.8698976f );
	VerifyTestResult( IsMostlyEqual( noXRotation, identityValues ), "XRotationDegrees matrix should be identity for rotationDegrees=0.0" );
	VerifyTestResult( IsMostlyEqual( xRotation, xRotationValues ), "XRotationDegrees matrix incorrect for 36.8698976 degrees - jBasis should be (0,.8,.6), etc." );

#endif
	return 18; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixAccessors()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixAccessors )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float identityValues[16]	= { 1.f, 0.f, 0.f, 0.f,   0.f, 1.f, 0.f, 0.f,   0.f, 0.f, 1.f, 0.f,     0.f, 0.f, 0.f, 1.f };
	float uniqueValues[16]		= { 1.f, 2.f, 3.f, 4.f,   5.f, 6.f, 7.f, 8.f,   9.f, 10.f, 11.f, 12.f,    13.f, 14.f, 15.f, 16.f };

	Matrix44Class matrixConstructedFromFloats1( uniqueValues );
	Matrix44Class matrixConstructedFromFloats2( uniqueValues );
	const Matrix44Class uniqueMatrix( uniqueValues );
	VerifyTestResult( IsMostlyEqual( uniqueMatrix, uniqueValues ), "Matrix construct-from-16-floats must be correct in order for this test set to work" );

	Matrix44Class matrixMemcopiedDirectly;
	bool isSafeToMemcpy = (sizeof(Matrix44Class) == 64);
	if( isSafeToMemcpy )
	{
		memcpy( &matrixMemcopiedDirectly, uniqueValues, sizeof(uniqueValues) );
		VerifyTestResult( IsMostlyEqual( matrixMemcopiedDirectly, uniqueValues ), "Matrix did not have basis-major memory storage layout: Ix,Iy,Iz,Iw...Tx,Ty,Tz,Tw" );
	}
	else
	{
		VerifyTestResult( false, "Could not perform memcpy() test, since sizeof(Matrix) was wrong (should be 64 bytes)" );
	}

	float* matrixAsFloats2 = matrixConstructedFromFloats2.Mat44_GetAsFloatArray();
	const float* matrixAsFloats3 = uniqueMatrix.Mat44_GetAsConstFloatArray(); // Compile error?  Make sure you have a const version of Matrix::GetAsFloatArray() const!
	const Vector2Class iBasis2D = uniqueMatrix.Mat44_GetIBasis2D(); // Compile error?  Make sure these Get() methods are const!
	const Vector2Class jBasis2D = uniqueMatrix.Mat44_GetJBasis2D(); // Compile error?  Make sure these Get() methods are const!
	const Vector2Class tBasis2D = uniqueMatrix.Mat44_GetTranslation2D(); // Compile error?  Make sure these Get() methods are const!
	const Vector3Class iBasis3D = uniqueMatrix.Mat44_GetIBasis3D(); // Compile error?  Make sure these Get() methods are const!
	const Vector3Class jBasis3D = uniqueMatrix.Mat44_GetJBasis3D(); // Compile error?  Make sure these Get() methods are const!
	const Vector3Class kBasis3D = uniqueMatrix.Mat44_GetKBasis3D(); // Compile error?  Make sure these Get() methods are const!
	const Vector3Class tBasis3D = uniqueMatrix.Mat44_GetTranslation3D(); // Compile error?  Make sure these Get() methods are const!
	const Vector4Class iBasis4D = uniqueMatrix.Mat44_GetIBasis4D(); // Compile error?  Make sure these Get() methods are const!
	const Vector4Class jBasis4D = uniqueMatrix.Mat44_GetJBasis4D(); // Compile error?  Make sure these Get() methods are const!
	const Vector4Class kBasis4D = uniqueMatrix.Mat44_GetKBasis4D(); // Compile error?  Make sure these Get() methods are const!
	const Vector4Class tBasis4D = uniqueMatrix.Mat44_GetTranslation4D(); // Compile error?  Make sure these Get() methods are const!

	VerifyTestResult( IsMostlyEqual( matrixAsFloats2, uniqueValues, 16 ), "Matrix::GetAsFloatArray() (non-const) was incorrect" );
	VerifyTestResult( IsMostlyEqual( matrixAsFloats3, uniqueValues, 16 ), "Matrix::GetAsFloatArray() (const) was incorrect" );
	VerifyTestResult( IsMostlyEqual( iBasis2D, Vector2Class( 1.f, 2.f ) ), "Matrix::GetIBasis2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( jBasis2D, Vector2Class( 5.f, 6.f ) ), "Matrix::GetJBasis2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( tBasis2D, Vector2Class( 13.f, 14.f ) ), "Matrix::GetTranslation2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( iBasis3D, Vector3Class( 1.f, 2.f, 3.f ) ), "Matrix::GetIBasis3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( jBasis3D, Vector3Class( 5.f, 6.f, 7.f ) ), "Matrix::GetJBasis3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( kBasis3D, Vector3Class( 9.f, 10.f, 11.f ) ), "Matrix::GetKBasis3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( tBasis3D, Vector3Class( 13.f, 14.f, 15.f ) ), "Matrix::GetTranslation3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( iBasis4D, Vector4Class( 1.f, 2.f, 3.f, 4.f ) ), "Matrix::GetIBasis4D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( jBasis4D, Vector4Class( 5.f, 6.f, 7.f, 8.f ) ), "Matrix::GetJBasis4D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( kBasis4D, Vector4Class( 9.f, 10.f, 11.f, 12.f ) ), "Matrix::GetKBasis4D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( tBasis4D, Vector4Class( 13.f, 14.f, 15.f, 16.f ) ), "Matrix::GetTranslation4D() was incorrect" );

#endif
	return 15; // Number of tests expected
}

/*
//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixMutators()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixMutators )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float uniqueValues[16]		= { 1.f, 2.f, 3.f, 4.f,		5.f, 6.f, 7.f, 8.f,		9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float t2DValues[16]			= { 1.f, 2.f, 3.f, 4.f,		5.f, 6.f, 7.f, 8.f,		9.f, 10.f, 11.f, 12.f,		88.f, 99.f, 0.f, 1.f };
	float t3DValues[16]			= { 1.f, 2.f, 3.f, 4.f,		5.f, 6.f, 7.f, 8.f,		9.f, 10.f, 11.f, 12.f,		77.f, 88.f, 99.f, 1.f };
	float ij2DValues[16]		= { 66.f, 77.f, 0.f, 0.f,	88.f, 99.f, 0.f, 0.f,	9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float ijt2DValues[16]		= { 44.f, 55.f, 0.f, 0.f,	66.f, 77.f, 0.f, 0.f,	9.f, 10.f, 11.f, 12.f,		88.f, 99.f, 0.f, 1.f };
	float ijk3DValues[16]		= { 11.f, 22.f, 33.f, 0.f,	44.f, 55.f, 66.f, 0.f,	77.f, 88.f, 99.f, 0.f,		13.f, 14.f, 15.f, 16.f };
	float ijkt3DValues[16]		= { 11.f, 22.f, 33.f, 0.f,	44.f, 55.f, 66.f, 0.f,	77.f, 88.f, 99.f, 0.f,		12.f, 23.f, 34.f, 1.f };
	float ijkt4DValues[16]		= { 11.f, 22.f, 33.f, 44.f,	55.f, 66.f, 77.f, 88.f,	13.f, 24.f, 35.f, 46.f,		14.f, 25.f, 36.f, 47.f };

	Matrix44Class uniqueMatrix( uniqueValues );
	Matrix44Class t2D( uniqueValues );
	Matrix44Class t3D( uniqueValues );
	Matrix44Class ij2D( uniqueValues );
	Matrix44Class ijt2D( uniqueValues );
	Matrix44Class ijk3D( uniqueValues );
	Matrix44Class ijkt3D( uniqueValues );
	Matrix44Class ijkt4D( uniqueValues );

	t2D.Mat44_SetTranslation2D( Vector2Class( 88.f, 99.f ) );
	t3D.Mat44_SetTranslation3D( Vector3Class( 77.f, 88.f, 99.f ) );
	ij2D.Mat44_SetIJ2D( Vector2Class( 66.f, 77.f ), Vector2Class( 88.f, 99.f ) );
	ijt2D.Mat44_SetIJT2D( Vector2Class( 44.f, 55.f ), Vector2Class( 66.f, 77.f ), Vector2Class( 88.f, 99.f ) );
	ijk3D.Mat44_SetIJK3D( Vector3Class( 11.f, 22.f, 33.f ), Vector3Class( 44.f, 55.f, 66.f ), Vector3Class( 77.f, 88.f, 99.f ) );
	ijkt3D.Mat44_SetIJKT3D( Vector3Class( 11.f, 22.f, 33.f ), Vector3Class( 44.f, 55.f, 66.f ), Vector3Class( 77.f, 88.f, 99.f ), Vector3Class( 12.f, 23.f, 34.f ) );
	ijkt4D.Mat44_SetIJKT4D( Vector4Class( 11.f, 22.f, 33.f, 44.f ), Vector4Class( 55.f, 66.f, 77.f, 88.f ), Vector4Class( 13.f, 24.f, 35.f, 46.f ), Vector4Class( 14.f, 25.f, 36.f, 47.f ) );

	VerifyTestResult( IsMostlyEqual( uniqueMatrix, uniqueValues ), "Matrix construct-from-16-floats must be correct in order for this test set to work" );
	VerifyTestResult( IsMostlyEqual( t2D, t2DValues ), "Matrix SetTranslation2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( t3D, t3DValues ), "Matrix SetTranslation3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( ij2D, ij2DValues ), "Matrix SetIJ2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( ijt2D, ijt2DValues ), "Matrix SetIJT2D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( ijk3D, ijk3DValues ), "Matrix SetIJK3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( ijkt3D, ijkt3DValues ), "Matrix SetIJKT3D() was incorrect" );
	VerifyTestResult( IsMostlyEqual( ijkt4D, ijkt4DValues ), "Matrix SetIJKT4D() was incorrect" );

#endif
	return 8; // Number of tests expected
}
*/

//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixAppends()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixAppends )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float identityValues[16]				= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identTranslation2DValues[16]		= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   11.f, 22.f, 0.f, 1.f };
	float identTranslation3DValues[16]		= { 1.f, 0.f, 0.f, 0.f,		0.f, 1.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   33.f, 44.f, 55.f, 1.f };
	float identUniformScale2DValues[16]		= { 3.f, 0.f, 0.f, 0.f,		0.f, 3.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identUniformScale3DValues[16]		= { 4.f, 0.f, 0.f, 0.f,		0.f, 4.f, 0.f, 0.f,		0.f, 0.f, 4.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identNonUniformScale2DValues[16]	= { 5.f, 0.f, 0.f, 0.f,		0.f, 6.f, 0.f, 0.f,		0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identNonUniformScale3DValues[16]	= { 2.f, 0.f, 0.f, 0.f,		0.f, 3.f, 0.f, 0.f,		0.f, 0.f, 4.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identZRotationValues[16]			= { .6f, .8f, 0.f, 0.f,		-.8f, .6f, 0.f, 0.f,	0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identYRotationValues[16]			= { .6f, 0.f, -.8f, 0.f,	0.f, 1.f, 0.f, 0.f,		.8f, 0.f, .6f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float identXRotationValues[16]			= { 1.f, 0.f, 0.f, 0.f,		0.f, .6f, .8f, 0.f,		0.f, -.8f, .6f, 0.f,   0.f, 0.f, 0.f, 1.f };

	Matrix44Class identity;
	Matrix44Class identityTranslated2D;
	Matrix44Class identityTranslated3D;
	Matrix44Class identityUniScaled2D;
	Matrix44Class identityUniScaled3D;
	Matrix44Class identityNonScaled2D;
	Matrix44Class identityNonScaled3D;
	Matrix44Class identityRotatedZ;
	Matrix44Class identityRotatedY;
	Matrix44Class identityRotatedX;
	identityTranslated2D.Mat44_Translate2D( Vector2Class( 11.f, 22.f ) );
	identityTranslated3D.Mat44_Translate3D( Vector3Class( 33.f, 44.f, 55.f ) );
	identityUniScaled2D.Mat44_ScaleUniform2D( 3.f );
	identityUniScaled3D.Mat44_ScaleUniform3D( 4.f );
	identityNonScaled2D.Mat44_ScaleNonUniform2D( Vector2Class( 5.f, 6.f ) );
	identityNonScaled3D.Mat44_ScaleNonUniform3D( Vector3Class( 2.f, 3.f, 4.f ) );
	identityRotatedZ.Mat44_RotateZDegrees( 53.13010235f );
	identityRotatedY.Mat44_RotateYDegrees( 53.13010235f );
	identityRotatedX.Mat44_RotateXDegrees( 53.13010235f );

	VerifyTestResult( IsMostlyEqual( identity, identityValues ), "Matrix must default construct to Identity in order for this test set to work" );
	VerifyTestResult( IsMostlyEqual( identityTranslated2D, identTranslation2DValues ), "Matrix::Translate2D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityTranslated3D, identTranslation3DValues ), "Matrix::Translate3D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityUniScaled2D, identUniformScale2DValues ), "Matrix::ScaleUniform2D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityUniScaled3D, identUniformScale3DValues ), "Matrix::ScaleUniform3D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityNonScaled2D, identNonUniformScale2DValues ), "Matrix::ScaleNonUniform2D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityNonScaled3D, identNonUniformScale3DValues ), "Matrix::ScaleNonUniform3D() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityRotatedZ, identZRotationValues ), "Matrix::RotateZDegrees() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityRotatedY, identYRotationValues ), "Matrix::RotateYDegrees() was incorrect on Identity" );
	VerifyTestResult( IsMostlyEqual( identityRotatedX, identXRotationValues ), "Matrix::RotateXDegrees() was incorrect on Identity" );

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float uniqueValues[16]					= { 1.f, 2.f, 3.f, 4.f,			5.f, 6.f, 7.f, 8.f,			9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueTranslation2DValues[16]		= { 1.f, 2.f, 3.f, 4.f,			5.f, 6.f, 7.f, 8.f,			9.f, 10.f, 11.f, 12.f,		134.f, 168.f, 202.f, 236.f };
	float uniqueTranslation3DValues[16]		= { 1.f, 2.f, 3.f, 4.f,			5.f, 6.f, 7.f, 8.f,			9.f, 10.f, 11.f, 12.f,		761.f, 894.f, 1027.f, 1160.f };
	float uniqueUniformScale2DValues[16]	= { 3.f, 6.f, 9.f, 12.f,		15.f, 18.f, 21.f, 24.f,		9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueUniformScale3DValues[16]	= { 4.f, 8.f, 12.f, 16.f,		20.f, 24.f, 28.f, 32.f,		36.f, 40.f, 44.f, 48.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueNonUniformScale2DValues[16]	= { 5.f, 10.f, 15.f, 20.f,		30.f, 36.f, 42.f, 48.f,		9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueNonUniformScale3DValues[16]	= { 2.f, 4.f, 6.f, 8.f,			15.f, 18.f, 21.f, 24.f,		36.f, 40.f, 44.f, 48.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueZRotationValues[16]			= { 4.6f, 6.f, 7.4f, 8.8f,		2.2f, 2.f, 1.8f, 1.6f,		9.f, 10.f, 11.f, 12.f,		13.f, 14.f, 15.f, 16.f };
	float uniqueYRotationValues[16]			= { -6.6f, -6.8f, -7.f, -7.2f,	5.f, 6.f, 7.f, 8.f,			6.2f, 7.6f, 9.f, 10.4f,		13.f, 14.f, 15.f, 16.f };
	float uniqueXRotationValues[16]			= { 1.f, 2.f, 3.f, 4.f,			10.2f, 11.6f, 13.f, 14.4f,	1.4f, 1.2f, 1.f, .8f,		13.f, 14.f, 15.f, 16.f };
	float otherUniqueMatrixValues[16]		= { .2f, .3f, .4f, .5f,			.6f, .7f, .8f, .9f,			1.1f, 1.2f, 1.3f, 1.4f,		1.5f, 1.6f, 1.7f, 1.8f };
	float uniqueProductValues[16]			= { 11.8f, 13.2f, 14.6f, 16.f,	23.f, 26.f, 29.f, 32.f,		37.f, 42.f, 47.f, 52.f,		48.2f, 54.8f, 61.4f, 68.f };

	Matrix44Class uniqueMatrix( uniqueValues );
	Matrix44Class uniqueMatrix2( uniqueValues );
	Matrix44Class uniqueMatrix3; // initially Identity
	Matrix44Class uniqueProduct( uniqueValues );
	Matrix44Class uniqueTranslated2D( uniqueValues );
	Matrix44Class uniqueTranslated3D( uniqueValues );
	Matrix44Class uniqueUniScaled2D( uniqueValues );
	Matrix44Class uniqueUniScaled3D( uniqueValues );
	Matrix44Class uniqueNonScaled2D( uniqueValues );
	Matrix44Class uniqueNonScaled3D( uniqueValues );
	Matrix44Class uniqueRotatedZ( uniqueValues );
	Matrix44Class uniqueRotatedY( uniqueValues );
	Matrix44Class uniqueRotatedX( uniqueValues );
	uniqueTranslated2D.Mat44_Translate2D( Vector2Class( 11.f, 22.f ) );
	uniqueTranslated3D.Mat44_Translate3D( Vector3Class( 33.f, 44.f, 55.f ) );
	uniqueUniScaled2D.Mat44_ScaleUniform2D( 3.f );
	uniqueUniScaled3D.Mat44_ScaleUniform3D( 4.f );
	uniqueNonScaled2D.Mat44_ScaleNonUniform2D( Vector2Class( 5.f, 6.f ) );
	uniqueNonScaled3D.Mat44_ScaleNonUniform3D( Vector3Class( 2.f, 3.f, 4.f ) );
	uniqueRotatedZ.Mat44_RotateZDegrees( 53.13010235f );
	uniqueRotatedY.Mat44_RotateYDegrees( 53.13010235f );
	uniqueRotatedX.Mat44_RotateXDegrees( 53.13010235f );
	uniqueMatrix2.Mat44_TransformBy( identity );
	uniqueMatrix3.Mat44_TransformBy( uniqueMatrix );
	Matrix44Class otherUniqueMatrix( otherUniqueMatrixValues );
	uniqueProduct.Mat44_TransformBy( otherUniqueMatrix );

	VerifyTestResult( IsMostlyEqual( uniqueMatrix, uniqueValues ), "Matrix construct-from-16-floats must be correct in order for this test set to work" );
	VerifyTestResult( IsMostlyEqual( uniqueTranslated2D, uniqueTranslation2DValues ), "Matrix::Translate2D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueTranslated3D, uniqueTranslation3DValues ), "Matrix::Translate3D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueUniScaled2D, uniqueUniformScale2DValues ), "Matrix::ScaleUniform2D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueUniScaled3D, uniqueUniformScale3DValues ), "Matrix::ScaleUniform3D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueNonScaled2D, uniqueNonUniformScale2DValues ), "Matrix::ScaleNonUniform2D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueNonScaled3D, uniqueNonUniformScale3DValues ), "Matrix::ScaleNonUniform3D() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueRotatedZ, uniqueZRotationValues ), "Matrix::RotateZDegrees() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueRotatedY, uniqueYRotationValues ), "Matrix::RotateYDegrees() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueRotatedX, uniqueXRotationValues ), "Matrix::RotateXDegrees() was incorrect on arbitrary matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueMatrix3, uniqueValues ), "Matrix::TransformBy( identity ) should not change the matrix" );
	VerifyTestResult( IsMostlyEqual( uniqueProduct, uniqueProductValues ), "Matrix::TransformBy( funkyMatrix ) was incorrect" );

#endif
	return 22; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
int TestSet_MP1A5_MatrixTransforms()
{
#if defined( ENABLE_TestSet_MP1A5_MatrixTransforms )

	// These "correct" matrix values are given as:  Ix,Iy,Iz,Iw,   Jx,Jy,Jz,Jw,   Kx,Ky,Kz,Kw,   Tx,Ty,Tz,Tw
	float identityValues[16]		= { 1.f, 0.f, 0.f, 0.f,    0.f, 1.f, 0.f, 0.f,    0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float translation2DValues[16]	= { 1.f, 0.f, 0.f, 0.f,    0.f, 1.f, 0.f, 0.f,    0.f, 0.f, 1.f, 0.f,   5.f, 9.f, 0.f, 1.f };
	float uniformScale2DValues[16]	= { 7.f, 0.f, 0.f, 0.f,    0.f, 7.f, 0.f, 0.f,    0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float zRotation2DValues[16]		= { 0.8f, 0.6f, 0.f, 0.f, -0.6f, 0.8f, 0.f, 0.f,  0.f, 0.f, 1.f, 0.f,   0.f, 0.f, 0.f, 1.f };
	float complexMatrixValues[16]	= { 2.f, 3.f, 4.f, 5.f,    6.f, 7.f, 8.f, 9.f,    10.f, 11.f, 12.f, 13.f,  14.f, 15.f, 16.f, 17.f };

	Vector2Class vec2D( 5.f, 2.f );					// assume z=0, w can be 0 or 1 depending on function being called
	Vector3Class vec3D( 6.f, 3.f, 2.f );			// w can be 0 or 1 depending on function being called
	Vector4Class position4D( 7.f, 5.f, 3.f, 1.f );	// w = 1 for positions
	Vector4Class vector4D( 7.f, 5.f, 3.f, 0.f );	// w = 0 for directions

	Matrix44Class identity;
	Vector2Class v2a = identity.Mat44_TransformVector2D( vec2D );
	Vector2Class p2a = identity.Mat44_TransformPosition2D( vec2D );
	Vector3Class v3a = identity.Mat44_TransformVector3D( vec3D );
	Vector3Class p3a = identity.Mat44_TransformPosition3D( vec3D );
	Vector4Class v4a = identity.Mat44_TransformHomogeneousPoint3D( vector4D );
	Vector4Class p4a = identity.Mat44_TransformHomogeneousPoint3D( position4D );
	VerifyTestResult( IsMostlyEqual( v2a, vec2D ),		"Identity matrix should not modify point when calling TransformVector2D()" );
	VerifyTestResult( IsMostlyEqual( p2a, vec2D ),		"Identity matrix should not modify point when calling TransformPosition2D()" );
	VerifyTestResult( IsMostlyEqual( v3a, vec3D ),		"Identity matrix should not modify point when calling TransformVector3D()" );
	VerifyTestResult( IsMostlyEqual( p3a, vec3D ),		"Identity matrix should not modify point when calling TransformPosition3D()" );
	VerifyTestResult( IsMostlyEqual( v4a, vector4D ),	"Identity matrix should not modify point when calling TransformHomogeneousPoint3D()" );
	VerifyTestResult( IsMostlyEqual( p4a, position4D ), "Identity matrix should not modify point when calling TransformHomogeneousPoint3D()" );

	const Matrix44Class complexMatrix( complexMatrixValues );
	Vector2Class v2b = complexMatrix.Mat44_TransformVector2D( vec2D ); // Compile error?  Check your const-ness!
	Vector2Class p2b = complexMatrix.Mat44_TransformPosition2D( vec2D ); // Compile error?  Check your const-ness!
	Vector3Class v3b = complexMatrix.Mat44_TransformVector3D( vec3D ); // Compile error?  Check your const-ness!
	Vector3Class p3b = complexMatrix.Mat44_TransformPosition3D( vec3D ); // Compile error?  Check your const-ness!
	Vector4Class v4b = complexMatrix.Mat44_TransformHomogeneousPoint3D( vector4D ); // Compile error?  Check your const-ness!
	Vector4Class p4b = complexMatrix.Mat44_TransformHomogeneousPoint3D( position4D );
	Vector2Class v2bCorrect( 22.f, 29.f );
	Vector2Class p2bCorrect( 36.f, 44.f );
	Vector3Class v3bCorrect( 50.f, 61.f, 72.f );
	Vector3Class p3bCorrect( 64.f, 76.f, 88.f );
	Vector4Class v4bCorrect( 74.f, 89.f, 104.f, 119.f );
	Vector4Class p4bCorrect( 88.f, 104.f, 120.f, 136.f );
	VerifyTestResult( IsMostlyEqual( complexMatrix, complexMatrixValues ), "Matrix construct-from-16-floats must be correct in order for this test set to work" );
	VerifyTestResult( IsMostlyEqual( v2b, v2bCorrect ),	"incorrect result for complexMatrix.TransformVector2D()" );
	VerifyTestResult( IsMostlyEqual( p2b, p2bCorrect ),	"incorrect result for complexMatrix.TransformPosition2D()" );
	VerifyTestResult( IsMostlyEqual( v3b, v3bCorrect ),	"incorrect result for complexMatrix.TransformVector3D()" );
	VerifyTestResult( IsMostlyEqual( p3b, p3bCorrect ),	"incorrect result for complexMatrix.TransformPosition3D()" );
	VerifyTestResult( IsMostlyEqual( v4b, v4bCorrect ),	"incorrect result for complexMatrix.TransformHomogeneousPoint3D()" );
	VerifyTestResult( IsMostlyEqual( p4b, p4bCorrect ),	"incorrect result for complexMatrix.TransformHomogeneousPoint3D()" );

#endif
	return 13; // Number of tests expected
}


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A5()
{
	RunTestSet( true, TestSet_MP1A5_MatrixConstruction,		"MP1-A5: Matrix44 Construction" );
	RunTestSet( true, TestSet_MP1A5_MatrixStaticCreates,	"MP1-A5: Matrix44 Static Create methods" );
	RunTestSet( true, TestSet_MP1A5_MatrixAccessors,		"MP1-A5: Matrix44 Accessors (gets)" );

	// Note: I removed this test set, as there are some subjectivities and subtleties involved.
	// We will discuss in class.  You should still write the various Mat44::SetXXX() methods, but
	//	they will not be unit-tested nor graded (at least not for MP1-A5).  -prof. Eiserloh
//	RunTestSet( true, TestSet_MP1A5_MatrixMutators,			"MP1-A5: Matrix44 Mutators (sets)" );

	RunTestSet( true, TestSet_MP1A5_MatrixAppends,			"MP1-A5: Matrix44 Appends (self-transforms)" );
	RunTestSet( true, TestSet_MP1A5_MatrixTransforms,		"MP1-A5: Matrix44 Transform methods" );
}


