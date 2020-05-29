#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"



Mat44::Mat44( float* sixteenValuesBasisMajor )
{
	iX = sixteenValuesBasisMajor[0];
	iY = sixteenValuesBasisMajor[1];
	iZ = sixteenValuesBasisMajor[2];
	iW = sixteenValuesBasisMajor[3];

	jX = sixteenValuesBasisMajor[4];
	jY = sixteenValuesBasisMajor[5];
	jZ = sixteenValuesBasisMajor[6];
	jW = sixteenValuesBasisMajor[7];

	kX = sixteenValuesBasisMajor[8];
	kY = sixteenValuesBasisMajor[9];
	kZ = sixteenValuesBasisMajor[10];
	kW = sixteenValuesBasisMajor[11];

	tX = sixteenValuesBasisMajor[12];
	tY = sixteenValuesBasisMajor[13];
	tZ = sixteenValuesBasisMajor[14];
	tW = sixteenValuesBasisMajor[15];
}

Mat44::Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	iX = iBasis2D.x;
	iY = iBasis2D.y;

	jX = jBasis2D.x;
	jY = jBasis2D.y;

	tX = translation2D.x;
	tY = translation2D.y;
}

Mat44::Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	iX = iBasis3D.x;
	iY = iBasis3D.y;
	iZ = iBasis3D.z;

	jX = jBasis3D.x;
	jY = jBasis3D.y;
	jZ = jBasis3D.z;

	kX = kBasis3D.x;
	kY = kBasis3D.y;
	kZ = kBasis3D.z;

	tX = translation3D.x;
	tY = translation3D.y;
	tZ = translation3D.z;
}

Mat44::Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous )
{
	iX = iBasisHomogeneous.x;
	iY = iBasisHomogeneous.y;
	iZ = iBasisHomogeneous.z;
	iW = iBasisHomogeneous.w;

	jX = jBasisHomogeneous.x;
	jY = jBasisHomogeneous.y;
	jZ = jBasisHomogeneous.z;
	jW = jBasisHomogeneous.w;

	kX = kBasisHomogeneous.x;
	kY = kBasisHomogeneous.y;
	kZ = kBasisHomogeneous.z;
	kW = kBasisHomogeneous.w;

	tX = translationHomogeneous.x;
	tY = translationHomogeneous.y;
	tZ = translationHomogeneous.z;
	tW = translationHomogeneous.w;
}

bool Mat44::operator==( Mat44 const& compare ) const
{
	Mat44 c = compare;
	if( iX == c.iX &&
		iY == c.iY &&
		iZ == c.iZ &&
		iW == c.iW &&

		jX == c.jX &&
		jY == c.jY &&
		jZ == c.jZ &&
		jW == c.jW &&

		kX == c.kX &&
		kY == c.kY &&
		kZ == c.kZ &&
		kW == c.kW &&

		tX == c.tX &&
		tY == c.tY &&
		tZ == c.tZ &&
		tW == c.tW
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const Vec2 Mat44::TransformVector2D( const Vec2& vectorQuantity ) const
{
	Vec2 transformedVec;
	transformedVec.x = (iX * vectorQuantity.x) + (jX * vectorQuantity.y);
	transformedVec.y = (iY * vectorQuantity.x) + (jY * vectorQuantity.y);

	return transformedVec;
}

const Vec3 Mat44::TransformVector3D( const Vec3& vectorQuantity ) const
{
	Vec3 transformedVec;
	transformedVec.x = (iX * vectorQuantity.x) + (jX * vectorQuantity.y) + (kX * vectorQuantity.z);
	transformedVec.y = (iY * vectorQuantity.x) + (jY * vectorQuantity.y) + (kY * vectorQuantity.z);
	transformedVec.z = (iZ * vectorQuantity.x) + (jZ * vectorQuantity.y) + (kZ * vectorQuantity.z);

	return transformedVec;
}

const Vec2 Mat44::TransformPosition2D( const Vec2& position ) const
{
	Vec2 transformedVec;
	transformedVec.x = (iX * position.x) + (jX * position.y) + (tX * 1.f);
	transformedVec.y = (iY * position.x) + (jY * position.y) + (tY * 1.f);

	return transformedVec;
}

const Vec3 Mat44::TransformPosition3D( const Vec3& position ) const
{
	Vec3 transformedVec;
	transformedVec.x = (iX * position.x) + (jX * position.y) + (kX * position.z) + (tX * 1.f);
	transformedVec.y = (iY * position.x) + (jY * position.y) + (kY * position.z) + (tY * 1.f);
	transformedVec.z = (iZ * position.x) + (jZ * position.y) + (kZ * position.z) + (tZ * 1.f);

	return transformedVec;
}

const Vec4 Mat44::TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const
{
	Vec4 transformedVec;
	transformedVec.x = (iX * homogeneousPoint.x) + (jX * homogeneousPoint.y) + (kX * homogeneousPoint.z) + (tX * homogeneousPoint.w);
	transformedVec.y = (iY * homogeneousPoint.x) + (jY * homogeneousPoint.y) + (kY * homogeneousPoint.z) + (tY * homogeneousPoint.w);
	transformedVec.z = (iZ * homogeneousPoint.x) + (jZ * homogeneousPoint.y) + (kZ * homogeneousPoint.z) + (tZ * homogeneousPoint.w);
	transformedVec.w = (iW * homogeneousPoint.x) + (jW * homogeneousPoint.y) + (kW * homogeneousPoint.z) + (tW * homogeneousPoint.w);

	return transformedVec;
}

const Vec2 Mat44::GetIBasis2D() const
{
	Vec2 iBasis(iX,iY);
	return iBasis;
}

const Vec2 Mat44::GetJBasis2D() const
{
	Vec2 jBasis( jX, jY );
	return jBasis;
}

const Vec2 Mat44::GetTranslation2D() const
{
	Vec2 translated2D( tX, tY );
	return translated2D;
}

const Vec3 Mat44::GetIBasis3D() const
{
	Vec3 iBasis( iX, iY, iZ );
	return iBasis;
}

const Vec3 Mat44::GetJBasis3D() const
{
	Vec3 jBasis( jX, jY, jZ );
	return jBasis;
}

const Vec3 Mat44::GetKBasis3D() const
{
	Vec3 kBasis( kX, kY, kZ );
	return kBasis;
}

const Vec3 Mat44::GetTranslation3D() const
{
	Vec3 translated3D( tX, tY, tZ );
	return translated3D;
}

const Vec4 Mat44::GetIBasis4D() const
{
	Vec4 iBasis( iX, iY, iZ, iW );
	return iBasis;
}

const Vec4 Mat44::GetJBasis4D() const
{
	Vec4 jBasis( jX, jY, jZ, jW );
	return jBasis;
}

const Vec4 Mat44::GetKBasis4D() const
{
	Vec4 kBasis( kX, kY, kZ, kW );
	return kBasis;
}

const Vec4 Mat44::GetTranslation4D() const
{
	Vec4 translated4D( tX, tY, tZ, tW );
	return translated4D;
}

void Mat44::SetTranslation2D( const Vec2& translation2D )
{
	tX = translation2D.x;
	tY = translation2D.y;
}

void Mat44::SetTranslation3D( const Vec3& translation3D )
{
	tX = translation3D.x;
	tY = translation3D.y;
	tZ = translation3D.z;
}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D )
{
	iX = iBasis2D.x;
	iY = iBasis2D.y;
	iZ = 0.f;
	iW = 0.f;

	jX = jBasis2D.x;
	jY = jBasis2D.y;
	jZ = 0.f;
	jW = 0.f;

}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	iX = iBasis2D.x;
	iY = iBasis2D.y;
	iZ = 0.f;
	iW = 0.f;

	jX = jBasis2D.x;
	jY = jBasis2D.y;
	jZ = 0.f;
	jW = 0.f;

	tX = translation2D.x;
	tY = translation2D.y;
	tZ = 0.f;
	tW = 1.f;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D )
{
		iX = iBasis3D.x;
		iY = iBasis3D.y;
		iZ = iBasis3D.z;
		iW = 0.f;

		jX = jBasis3D.x;
		jY = jBasis3D.y;
		jZ = jBasis3D.z;
		jW = 0.f;

		kX = kBasis3D.x;
		kY = kBasis3D.y;
		kZ = kBasis3D.z;
		kW = 0.f;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	iX = iBasis3D.x;
	iY = iBasis3D.y;
	iZ = iBasis3D.z;
	iW = 0.f;

	jX = jBasis3D.x;
	jY = jBasis3D.y;
	jZ = jBasis3D.z;
	jW = 0.f;

	kX = kBasis3D.x;
	kY = kBasis3D.y;
	kZ = kBasis3D.z;
	kW = 0.f;

	tX = translation3D.x;
	tY = translation3D.y;
	tZ = translation3D.z;
	tW = 1.f;
}

void Mat44::SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D )
{
	iX = iBasis4D.x;
	iY = iBasis4D.y;
	iZ = iBasis4D.z;
	iW = iBasis4D.w;

	jX = jBasis4D.x;
	jY = jBasis4D.y;
	jZ = jBasis4D.z;
	jW = jBasis4D.w;

	kX = kBasis4D.x;
	kY = kBasis4D.y;
	kZ = kBasis4D.z;
	kW = kBasis4D.w;

	tX = translation4D.x;
	tY = translation4D.y;
	tZ = translation4D.z;
	tW = translation4D.w;
}

void Mat44::RotateXDegrees( float degreesAboutX )
{
	float c = CosDegrees( degreesAboutX );
	float s = SinDegrees( degreesAboutX );

	//c, -s
	//s, c

/*	TransformBy( Mat44::CreateXRotationDegrees(degreesAboutX) );*/

	float njX = (jX * c) + (kX * s);
	float njY = (jY * c) + (kY * s);
	float njZ = (jZ * c) + (kZ * s);
	float njW = (jW * c) + (kW * s);

	float nkX = (jX * -s) + (kX * c);
	float nkY = (jY * -s) + (kY * c);
	float nkZ = (jZ * -s) + (kZ * c);
	float nkW = (jW * -s) + (kW * c);


	jX = njX;
	jY = njY;
	jZ = njZ;
	jW = njW;

	kX = nkX;
	kY = nkY;
	kZ = nkZ;
	kW = nkW;
	
}

void Mat44::RotateYDegrees( float degreesAboutY )
{
	float c = CosDegrees( degreesAboutY );
	float s = SinDegrees( degreesAboutY );

	//c, -s
	//s, c

/*	TransformBy( Mat44::CreateYRotationDegrees(degreesAboutY) );*/


	float niX = (iX * c) + (kX * -s);
	float niY = (iY * c) + (kY * -s);
	float niZ = (iZ * c) + (kZ * -s);
	float niW = (iW * c) + (kW * -s);

	float nkX = (iX * s) + (kX * c);
	float nkY = (iY * s) + (kY * c);
	float nkZ = (iZ * s) + (kZ * c);
	float nkW = (iW * s) + (kW * c);


	iX = niX;
	iY = niY;
	iZ = niZ;
	iW = niW;

	kX = nkX;
	kY = nkY;
	kZ = nkZ;
	kW = nkW;

}

void Mat44::RotateZDegrees( float degreesAboutZ )
{
	float c = CosDegrees(degreesAboutZ);
	float s = SinDegrees(degreesAboutZ);

	//c, -s
	//s, c

	float nIX = (iX * c) + (jX * s);
	float nIY = (iY * c) + (jY * s);
	float nIZ = (iZ * c) + (jZ * s);
	float nIW = (iW * c) + (jW * s);

	float nJX = (iX * -s) + (jX * c);
	float nJY = (iY * -s) + (jY * c);
	float nJZ = (iZ * -s) + (jZ * c);
	float nJW = (iW * -s) + (jW * c);



	iX = nIX;
	iY = nIY;
	iZ = nIZ;
	iW = nIW;

	jX = nJX;
	jY = nJY;
	jZ = nJZ;
	jW = nJW;
}

void Mat44::RotateYawPitchRollDegress( float yaw, float pitch, float roll )
{
	//order is flipped because we are "pushing onto the stack"
	//Should always be roll, pitch, yaw with differing Rotate methods called
	if( g_currentBases == eYawPitchRollRotationOrder::YXZ )
	{
		RotateZDegrees( roll );
		RotateXDegrees( pitch );
		RotateYDegrees( yaw );
	}
	else if( g_currentBases == eYawPitchRollRotationOrder::ZYX )
	{
		RotateXDegrees( roll );
		RotateZDegrees( yaw );
		RotateYDegrees( pitch );
	}
	else
	{
		//current
		RotateZDegrees( roll );
		RotateYDegrees( pitch );
		RotateXDegrees( yaw );
	}
}

void Mat44::Translate2D( const Vec2& translationXY )
{
	float nTX = (iX * translationXY.x) + (jX * translationXY.y) + (tX * 1.f);
	float nTY = (iY * translationXY.x) + (jY * translationXY.y) + (tY * 1.f);
	float nTZ = (iZ * translationXY.x) + (jZ * translationXY.y) + (tZ * 1.f);
	float nTW = (iW * translationXY.x) + (jW * translationXY.y) + (tW * 1.f);


	tX = nTX;
	tY = nTY;
	tZ = nTZ;
	tW = nTW;
}

void Mat44::Translate3D( const Vec3& translation3D )
{
	float nTX = (iX * translation3D.x) + (jX * translation3D.y) + (kX * translation3D.z) + (tX * 1.f);
	float nTY = (iY * translation3D.x) + (jY * translation3D.y) + (kY * translation3D.z) + (tY * 1.f);
	float nTZ = (iZ * translation3D.x) + (jZ * translation3D.y) + (kZ * translation3D.z) + (tZ * 1.f);
	float nTW = (iW * translation3D.x) + (jW * translation3D.y) + (kW * translation3D.z) + (tW * 1.f);


	tX = nTX;
	tY = nTY;
	tZ = nTZ;
	tW = nTW;
}

void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	iX *= uniformScaleXY;
	iY *= uniformScaleXY;
	iZ *= uniformScaleXY;
	iW *= uniformScaleXY;

	jX *= uniformScaleXY;
	jY *= uniformScaleXY;
	jZ *= uniformScaleXY;
	jW *= uniformScaleXY;

}

void Mat44::ScaleNonUniform2D( const Vec2& scaleFactorsXY )
{
	iX *= scaleFactorsXY.x;
	iY *= scaleFactorsXY.x;
	iZ *= scaleFactorsXY.x;
	iW *= scaleFactorsXY.x;

	jX *= scaleFactorsXY.y;
	jY *= scaleFactorsXY.y;
	jZ *= scaleFactorsXY.y;
	jW *= scaleFactorsXY.y;
}

void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	iX *= uniformScaleXYZ;
	iY *= uniformScaleXYZ;
	iZ *= uniformScaleXYZ;
	iW *= uniformScaleXYZ;

	jX *= uniformScaleXYZ;
	jY *= uniformScaleXYZ;
	jZ *= uniformScaleXYZ;
	jW *= uniformScaleXYZ;

	kX *= uniformScaleXYZ;
	kY *= uniformScaleXYZ;
	kZ *= uniformScaleXYZ;
	kW *= uniformScaleXYZ;
}

void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	iX *= scaleFactorsXYZ.x;
	iY *= scaleFactorsXYZ.x;
	iZ *= scaleFactorsXYZ.x;
	iW *= scaleFactorsXYZ.x;

	jX *= scaleFactorsXYZ.y;
	jY *= scaleFactorsXYZ.y;
	jZ *= scaleFactorsXYZ.y;
	jW *= scaleFactorsXYZ.y;

	kX *= scaleFactorsXYZ.z;
	kY *= scaleFactorsXYZ.z;
	kZ *= scaleFactorsXYZ.z;
	kW *= scaleFactorsXYZ.z;
}

void Mat44::TransformBy( const Mat44& arbitraryTransformationToAppend )
{
	const Mat44& a = arbitraryTransformationToAppend;

	Mat44 n;

	n.iX = (iX * a.iX) + (jX * a.iY) + (kX * a.iZ) + (tX * a.iW);
	n.iY = (iY * a.iX) + (jY * a.iY) + (kY * a.iZ) + (tY * a.iW);
	n.iZ = (iZ * a.iX) + (jZ * a.iY) + (kZ * a.iZ) + (tZ * a.iW);
	n.iW = (iW * a.iX) + (jW * a.iY) + (kW * a.iZ) + (tW * a.iW);

	n.jX = (iX * a.jX) + (jX * a.jY) + (kX * a.jZ) + (tX * a.jW);
	n.jY = (iY * a.jX) + (jY * a.jY) + (kY * a.jZ) + (tY * a.jW);
	n.jZ = (iZ * a.jX) + (jZ * a.jY) + (kZ * a.jZ) + (tZ * a.jW);
	n.jW = (iW * a.jX) + (jW * a.jY) + (kW * a.jZ) + (tW * a.jW);

	n.kX = (iX * a.kX) + (jX * a.kY) + (kX * a.kZ) + (tX * a.kW);
	n.kY = (iY * a.kX) + (jY * a.kY) + (kY * a.kZ) + (tY * a.kW);
	n.kZ = (iZ * a.kX) + (jZ * a.kY) + (kZ * a.kZ) + (tZ * a.kW);
	n.kW = (iW * a.kX) + (jW * a.kY) + (kW * a.kZ) + (tW * a.kW);

	n.tX = (iX * a.tX) + (jX * a.tY) + (kX * a.tZ) + (tX * a.tW);
	n.tY = (iY * a.tX) + (jY * a.tY) + (kY * a.tZ) + (tY * a.tW);
	n.tZ = (iZ * a.tX) + (jZ * a.tY) + (kZ * a.tZ) + (tZ * a.tW);
	n.tW = (iW * a.tX) + (jW * a.tY) + (kW * a.tZ) + (tW * a.tW);

	*this = n;

}

const Mat44 Mat44::CreateXRotationDegrees( float degreesAboutX )
{
	Mat44 matrix;
	matrix.RotateXDegrees( degreesAboutX );
	return matrix;
}

const Mat44 Mat44::CreateYRotationDegrees( float degreesAboutY )
{
	Mat44 matrix;
	matrix.RotateYDegrees( degreesAboutY );
	return matrix;
}

const Mat44 Mat44::CreateZRotationDegrees( float degreesAboutZ )
{
	Mat44 matrix;
	matrix.RotateZDegrees( degreesAboutZ );
	return matrix;
}

const Mat44 Mat44::CreateTranslationXY( const Vec2& translationXY )
{
	Mat44 matrix;
	matrix.SetTranslation2D(translationXY);
	return matrix;
}

const Mat44 Mat44::CreateTranslation3D( const Vec3& translation3D )
{
	Mat44 matrix;
	matrix.SetTranslation3D(translation3D);
	return matrix;
}

const Mat44 Mat44::CreateUniformScaleXY( float uniformScaleXY )
{
	Mat44 matrix;
	matrix.ScaleUniform2D(uniformScaleXY);
	return matrix;
}

const Mat44 Mat44::CreateNonUniformScaleXY( const Vec2& scaleFactorsXY )
{
	Mat44 matrix;
	matrix.ScaleNonUniform2D(scaleFactorsXY);
	return matrix;
}

const Mat44 Mat44::CreateUniformScale3D( float uniformScale3D )
{
	Mat44 matrix;
	matrix.ScaleUniform3D(uniformScale3D);
	return matrix;
}

const Mat44 Mat44::CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 matrix;
	matrix.ScaleNonUniform3D(scaleFactorsXYZ);
	return matrix;
}

const Mat44 Mat44::CreateFromScaleRotationTranslation( Vec3 const& scale, Vec3 const& rotation, Vec3 const& translation )
{
	Mat44 mat;
	mat.CreateNonUniformScale3D( scale );
	mat.RotateXDegrees( rotation.x );
	mat.RotateYDegrees( rotation.y );
	mat.RotateZDegrees( rotation.z );
	mat.Translate3D( translation );

	return mat;
}

const Mat44 Mat44::CreateOrthographicProjection( const Vec3& min, const Vec3& max )
{
	// think of x
	// min.x, max.x -> (-1, 1)
	//ndc.x = (x - min.x) / (max.x - min.x) * (1.f - (-1.f)) + (-1.f)
	//ndc.x = x / (max.x - min.x) - (min.x / (max.x - min.x)) * 2.f + -1.f
	//a = 1.f/(max.x - min.x)
	//b = (-2.f * min.x - max.x + min.x) / (max.x - min.x)
	//	= -(max.x + min.x) / (max.x - min.x)

	// min.x, max.x -> (-1, 1)
	//ndc.x = (x - min.x) / (max.x - min.x) * (1.f - (0.f)) + (0.f)
	//ndc.x = x / (max.x - min.x) - (min.x / (max.x - min.x)) * 1.f
	//a = 1.f/(max.x - min.x)
	//b = -min.x / (max.x - min.x)

	Vec3 diff	= max - min;
	Vec3 sum	= max + min;

	float mat[] =
	{
		2.f / diff.x,		0.f,				0.f,				0.f,
		0.f,				2.f / diff.y,		0.f,				0.f,
		0.f,				0.f,				2.f / diff.z,		0.f,
		-sum.x / diff.x,	-sum.y / diff.y,	-min.z / diff.z,	1.f
	};

	return Mat44(mat);
}

