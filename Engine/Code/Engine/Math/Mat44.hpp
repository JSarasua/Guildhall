#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

struct Mat44
{
public:
	float iX = 1.f;
	float iY = 0.f;
	float iZ = 0.f;
	float iW = 0.f;

	float jX = 0.f;
	float jY = 1.f;
	float jZ = 0.f;
	float jW = 0.f;

	float kX = 0.f;
	float kY = 0.f;
	float kZ = 1.f;
	float kW = 0.f;

	float tX = 0.f;
	float tY = 0.f;
	float tZ = 0.f;
	float tW = 1.f;


public:
	Mat44() = default; //Default constructor is identity
	explicit Mat44( float* sixteenValuesBasisMajor );
	explicit Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D );
	explicit Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D );
	explicit Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous );


	//Transform position and vector quantities
	const Vec2 TransformVector2D( const Vec2& vectorQuantity ) const;	//Assume z = 0, w = 0
	const Vec3 TransformVector3D( const Vec3& vectorQuantity ) const;	//Assume w = 0
	const Vec2 TransformPosition2D( const Vec2& position ) const;		//Assume z = 0, w = 0
	const Vec3 TransformPosition3D( const Vec3& position ) const;		//Assume w = 1
	const Vec4 TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const; //explicit w= 0,1, or other

	const float*	GetAsFloatArray() const	{ return &iX; }
	float*			GetAsFloatArray()		{ return &iX; }
	const Vec2		GetIBasis2D() const;
	const Vec2		GetJBasis2D() const;
	const Vec2		GetTranslation2D() const;
	const Vec3		GetIBasis3D() const;
	const Vec3		GetJBasis3D() const;
	const Vec3		GetKBasis3D() const;
	const Vec3		GetTranslation3D() const;
 	const Vec4		GetIBasis4D() const;
 	const Vec4		GetJBasis4D() const;
 	const Vec4		GetKBasis4D() const;
 	const Vec4		GetTranslation4D() const;


	//Basic Mutators
	void SetTranslation2D( const Vec2& translation2D );
	void SetTranslation3D( const Vec3& translation3D );
	void SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D );
	void SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D );
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D );
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D );
	void SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D );

	//Transformation mutators; append (concatenation) a new transform to this matrix
	void RotateXDegrees( float degreesAboutX );
	void RotateYDegrees( float degreesAboutY );
	void RotateZDegrees( float degreesAboutZ );
	void Translate2D( const Vec2& translationXY );
	void Translate3D( const Vec3& translation3D );
	void ScaleUniform2D( float uniformScaleXY );
	void ScaleNonUniform2D( const Vec2& scaleFactorsXY );
	void ScaleUniform3D( float uniformScaleXYZ );
	void ScaleNonUniform3D( const Vec3& scaleFactorsXYZ );
	void TransformBy( const Mat44& arbitraryTransformationToAppend );

	//Static creation methods to create a matrix of a certain transformation type
	static const Mat44	CreateXRotationDegrees( float degreesAboutX );
	static const Mat44	CreateYRotationDegrees( float degreesAboutY );
	static const Mat44	CreateZRotationDegrees( float degreesAboutZ );
	static const Mat44	CreateTranslationXY( const Vec2& translationXY );
	static const Mat44	CreateTranslation3D( const Vec3& translation3D );
	static const Mat44	CreateUniformScaleXY( float uniformScaleXY );
	static const Mat44	CreateNonUniformScaleXY( const Vec2& scaleFactorsXY );
	static const Mat44	CreateUniformScale3D( float uniformScale3D );
	static const Mat44	CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ);
	static const Mat44	CreateFromScaleRotationTranslation( Vec3 const& scale, Vec3 const& rotation, Vec3 const& translation );

	// projection
	static const Mat44 CreateOrthographicProjection( const Vec3& min, const Vec3& max );

private:
	const Mat44 operator*( const Mat44& rhs ) const = delete; //FORBIDDEN
};