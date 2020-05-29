#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <math.h>

Mat44 MakeOrthographicProjectionMatrixD3D( float minX, float maxX, float minY, float maxY, float minZ, float maxZ )
{
	Vec3 min;
	Vec3 max;

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;

	return Mat44::CreateOrthographicProjection( min, max );
}

Mat44 MakePerspectiveProjectMatrixD3D( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	// float fov is the field of view you want
	// float farZ and nearZ are the depth range you want to see
	// -> do **not** span zero here.

	// Goal is to...
	// - setup a default "depth" where (1, 1) == (1, 1) after proejction (1 / tan(fov * .5f))
	// - map z to w, so the z divide happens
	// - map nearZ to 0, farZ to farZ, since a Z divide will happen
	//   and this will result in mapping nearZ to 0, and farZ to 1. 
	//   -> ((z - nz) / (fz - nz)) * fz + 0
	//   -> fz / (fz - nz) * z      + (-fz * nz) / (fz - nz)

	float height = 1.0f / TanDegrees( fovDegrees * .5f ); // how far away are we for the perspective point to be "one up" from our forward line. 
	float zrange = farZ - nearZ;
	float q = 1.0f / zrange;


	float sixteenValuesBasisMajor[16] =
	{
		height / aspectRatio,	0.f,	0.f,					0.f,
		0.f,					height, 0.f,					0.f,
		0.f,					0.f,	-farZ* q,				-1.f,
		0.f,					0.f,	nearZ * farZ * q,		0.f
	};



// 	float sixteenValuesBasisMajor[16] =
// 	{
// 		farZ* q,				0.f,					0.f,					-1.f,
// 		0.f,					height / aspectRatio,	0.f,					0.f,
// 		0.f,					0.f,					height,					0.f,
// 		nearZ * farZ * q,		0.f,					0.f,					0.f
// 	};

	Mat44 proj( sixteenValuesBasisMajor	);
	return proj;
}

void MatrixTranspose( Mat44& mat )
{
	Mat44 tempMat = mat;

	tempMat.iX = mat.iX;
	tempMat.iY = mat.jX;
	tempMat.iZ = mat.kX;
	tempMat.iW = mat.tX;

	tempMat.jX = mat.iY;
	tempMat.jY = mat.jY;
	tempMat.jZ = mat.kY;
	tempMat.jW = mat.tY;

	tempMat.kX = mat.iZ;
	tempMat.kY = mat.jZ;
	tempMat.kZ = mat.kZ;
	tempMat.kW = mat.tZ;

	tempMat.tX = mat.iW;
	tempMat.tY = mat.jW;
	tempMat.tZ = mat.kW;
	tempMat.tW = mat.tW;

	mat = tempMat;
}

void MatrixInvertOrthoNormal( Mat44& mat )
{
	Mat44 inverse = mat;
	inverse.SetTranslation3D( Vec3(0.f, 0.f, 0.f) );
	MatrixTranspose( inverse );

	Vec3 translation = mat.GetTranslation3D();
	//Vec3 inverseTranslation = translation * -1.f;
	Vec3 inverseTranslation = inverse.TransformPosition3D( -translation );
	inverse.SetTranslation3D( inverseTranslation );

	Mat44 identityFromInversion = inverse;
	identityFromInversion.TransformBy( mat );

	GUARANTEE_OR_DIE( IsMatrixEqual( identityFromInversion, Mat44(), 0.001f ), "Tried to invert a matrix with orthonormal inversion on a non orthonormal matrix" );

	mat = inverse;
}

void MatrixInvert( Mat44& mat )
{
	double inv[16];
	double det;
	double m[16];
	unsigned int i;

	float* matAsArray = mat.GetAsFloatArray();

	for( i = 0; i < 16; ++i ) {
		m[i] = (double)matAsArray[i];
	}

	inv[0] = m[5]  * m[10] * m[15] -
		m[5]  * m[11] * m[14] -
		m[9]  * m[6]  * m[15] +
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] -
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
		m[4]  * m[11] * m[14] +
		m[8]  * m[6]  * m[15] -
		m[8]  * m[7]  * m[14] -
		m[12] * m[6]  * m[11] +
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9]  * m[15] -
		m[4]  * m[11] * m[13] -
		m[8]  * m[5]  * m[15] +
		m[8]  * m[7]  * m[13] +
		m[12] * m[5]  * m[11] -
		m[12] * m[7]  * m[9];

	inv[12] = -m[4]  * m[9]  * m[14] +
		m[4]  * m[10] * m[13] +
		m[8]  * m[5]  * m[14] -
		m[8]  * m[6]  * m[13] -
		m[12] * m[5]  * m[10]  +
		m[12] * m[6]  * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
		m[1]  * m[11] * m[14] +
		m[9]  * m[2]  * m[15] -
		m[9]  * m[3]  * m[14] -
		m[13] * m[2]  * m[11] +
		m[13] * m[3]  * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
		m[0]  * m[11] * m[14] -
		m[8]  * m[2]  * m[15] +
		m[8]  * m[3]  * m[14] +
		m[12] * m[2]  * m[11] -
		m[12] * m[3]  * m[10];

	inv[9] = -m[0]  * m[9]  * m[15] +
		m[0]  * m[11] * m[13] +
		m[8]  * m[1]  * m[15] -
		m[8]  * m[3]  * m[13] -
		m[12] * m[1]  * m[11] +
		m[12] * m[3]  * m[9];

	inv[13] = m[0]  * m[9]  * m[14] -
		m[0]  * m[10] * m[13] -
		m[8]  * m[1]  * m[14] +
		m[8]  * m[2]  * m[13] +
		m[12] * m[1]  * m[10] -
		m[12] * m[2]  * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
		m[1]  * m[7] * m[14] -
		m[5]  * m[2] * m[15] +
		m[5]  * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
		m[0]  * m[7] * m[14] +
		m[4]  * m[2] * m[15] -
		m[4]  * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
		m[0]  * m[7] * m[13] -
		m[4]  * m[1] * m[15] +
		m[4]  * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
		m[0]  * m[6] * m[13] +
		m[4]  * m[1] * m[14] -
		m[4]  * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	for( i = 0; i < 16; i++ ) {
		matAsArray[i] = (float)(inv[i] * det);
	}

}

bool MatrixIsOrthoNormal( Mat44 const& mat )
{
	Mat44 invertedMat = mat;
	MatrixInvertOrthoNormal( invertedMat );

	invertedMat.TransformBy( mat );
	Mat44 identityMatrix;

	if( IsMatrixEqual( invertedMat, identityMatrix, 0.01f ) )
	{
		return true;
	}

	return false;
}

bool IsMatrixEqual( Mat44 const& matA, Mat44 const& matB, float epsilon /*= 0.01f */ )
{
	Mat44 a = matA;
	Mat44 b = matB;
	

	if( AlmostEqualsFloat(a.iX, b.iX, epsilon) &&
		AlmostEqualsFloat(a.iY, b.iY, epsilon) &&
		AlmostEqualsFloat(a.iZ, b.iZ, epsilon) &&
		AlmostEqualsFloat(a.iW, b.iW, epsilon) &&

		AlmostEqualsFloat(a.jX, b.jX, epsilon) &&
		AlmostEqualsFloat(a.jY, b.jY, epsilon) &&
		AlmostEqualsFloat(a.jZ, b.jZ, epsilon) &&
		AlmostEqualsFloat(a.jW, b.jW, epsilon) &&

		AlmostEqualsFloat(a.kX, b.kX, epsilon) &&
		AlmostEqualsFloat(a.kY, b.kY, epsilon) &&
		AlmostEqualsFloat(a.kZ, b.kZ, epsilon) &&
		AlmostEqualsFloat(a.kW, b.kW, epsilon) &&

		AlmostEqualsFloat(a.tX, b.tX, epsilon) &&
		AlmostEqualsFloat(a.tY, b.tY, epsilon) &&
		AlmostEqualsFloat(a.tZ, b.tZ, epsilon) &&
		AlmostEqualsFloat(a.tW, b.tW, epsilon)
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Mat44 LookAt( Vec3 const& worldLocation, Vec3 const& target, Vec3 const& worldUp )
{
	Vec3 forward = target - worldLocation;
	forward.Normalize();

	Vec3 right = CrossProduct( forward, worldUp );
	right.Normalize();

	Vec3 up = CrossProduct( right, forward );

	//Fix this, it assumes worldUp is 0,1,0 and is probably not correct in the first place
	float rightLength = right.GetLength();
	if( AlmostEqualsFloat( rightLength, 0.f ) )
	{
		forward = Vec3( 0.f, 1.f, 0.f );
		right = Vec3( 1.f, 0.f, 0.f );
		up = Vec3( 0.f, 0.f, 1.f );
	}


	Mat44 lookatMatrix;
	lookatMatrix.SetBasisVectors3D( right, up, -forward );
	
	return lookatMatrix;
}

//LookAt + translate by worldLocation
Mat44 LookAtAndMoveToWorld( Vec3 const& worldLocation, Vec3 const& target, Vec3 const& worldUp )
{
	Vec3 forward = target - worldLocation;
	forward.Normalize();

	Vec3 right = CrossProduct( forward, worldUp );
	right.Normalize();

	Vec3 up = CrossProduct( right, forward );

	//Fix this, it assumes worldUp is 0,1,0 and is probably not correct in the first place
	float rightLength = right.GetLength();

	Vec3 negativeForward = -forward;
	if( AlmostEqualsFloat( rightLength, 0.f ) )
	{
		if( forward.IsAlmostEqual( worldUp ) )
		{
			forward = Vec3( 0.f, 1.f, 0.f );
			right = Vec3( 1.f, 0.f, 0.f );
			up = Vec3( 0.f, 0.f, 1.f );
		}
		else if( negativeForward.IsAlmostEqual( worldUp ) )
		{
			forward = Vec3( 0.f, -1.f, 0.f );
			right = Vec3( 1.f, 0.f, 0.f );
			up = Vec3( 0.f, 0.f, -1.f );
		}
	}

	Mat44 lookatMatrix;
	lookatMatrix.SetBasisVectors3D( right, up, -forward );
	lookatMatrix.SetTranslation3D( worldLocation );


	return lookatMatrix;
}

