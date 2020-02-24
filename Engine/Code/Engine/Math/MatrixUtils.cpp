#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
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

	float height = 1.0f / tanf( fovDegrees * .5f ); // how far away are we for the perspective point to be "one up" from our forward line. 
	float zrange = farZ - nearZ;
	float q = 1.0f / zrange;


	float sixteenValuesBasisMajor[16] =
	{
		height / aspectRatio,	0.f,	0.f,		0.f,
		0.f,					height, 0.f,		0.f,
		0.f,					0.f,	-farZ* q,	nearZ * farZ * q,
		0.f,					0.f,	-1.f,		0.f
	};
	Mat44 proj( sixteenValuesBasisMajor	);
	return proj;
}

void MatrixTranspose( Mat44& mat )
{

}

void MatrixInvertOrthoNormal( Mat44& mat )
{

}

void MatrixInvert( Mat44& mat )
{

}

