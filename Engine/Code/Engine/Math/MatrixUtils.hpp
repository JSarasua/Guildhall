#pragma once
#include "Engine/Math/Mat44.hpp"

Mat44 MakeOrthographicProjectionMatrixD3D(
	float minX, float maxX,
	float minY, float maxY,
	float minZ, float maxZ );

Mat44 MakePerspectiveProjectMatrixD3D(
	float fovDegrees,
	float aspectRatio,
	float nearZ,
	float farZ );

//More Utility
void MatrixTranspose( Mat44& mat );
void MatrixInvertOrthoNormal( Mat44& mat );
void MatrixInvert( Mat44& mat );