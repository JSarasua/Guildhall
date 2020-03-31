#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x )
	, y( copy.y )
{}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( IntVec2 const& copyFrom )
	: x( static_cast<float>(copyFrom.x))
	, y( static_cast<float>(copyFrom.y))
{}


Vec2::Vec2( Vec3 const& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
{}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( Vec2 const& vecToAdd ) const
{
	return Vec2(x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( x * (-1), y * (-1) );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( float const uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( float const uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, Vec2 const& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	if( x == compare.x && y == compare.y ) {
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	if ( !(x == compare.x && y == compare.y) ) {
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians( float orientationRadians, float radius )
{
	Vec2 newVec;
	newVec.x = radius * cosf(orientationRadians);
	newVec.y = radius * sinf(orientationRadians);
	return newVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees( float orientationDegrees, float radius )
{
	Vec2 newVec;
	newVec.x = radius * CosDegrees(orientationDegrees);
	newVec.y = radius * SinDegrees(orientationDegrees);
	return newVec;
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{


	return sqrtf( (x*x) + (y*y));
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return ( (x*x) + (y*y) );
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleRadians() const
{
	return atan2f(y,x);
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleDegrees() const
{
	return Atan2Degrees(y,x);
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotated90Degrees() const
{
	float radius = GetLength();
	float angle = Atan2Degrees(y,x);
	angle += 90;

	Vec2 rotatedVec;
	rotatedVec.x = radius * CosDegrees(angle);
	rotatedVec.y = radius * SinDegrees(angle);
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	float radius = GetLength();
	float angle = Atan2Degrees( y, x );
	angle -= 90;

	Vec2 rotatedVec;
	rotatedVec.x = radius * CosDegrees( angle );
	rotatedVec.y = radius * SinDegrees( angle );
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotatedRadians( float deltaRadians ) const
{
	float radius = GetLength();
	float angle = atan2f( y, x );
	angle += deltaRadians;

	Vec2 rotatedVec;
	rotatedVec.x = radius * cosf( angle );
	rotatedVec.y = radius * sinf( angle );
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetRotatedDegrees( float deltaDegrees ) const
{
	float radius = GetLength();
	float angle = Atan2Degrees( y, x );
	angle += deltaDegrees;

	Vec2 rotatedVec;
	rotatedVec.x = radius * CosDegrees( angle );
	rotatedVec.y = radius * SinDegrees( angle );
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetClamped( float maxLength ) const
{
	float radius = GetLength();
	float angle = atan2f( y, x );

	Vec2 clampedVec;

	if( maxLength < radius )
	{
		clampedVec.x = maxLength * cosf( angle );
		clampedVec.y = maxLength * sinf( angle );
	}
	else
	{
		clampedVec.x = x;
		clampedVec.y = y;
	}


	return clampedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetNormalized() const
{
	float angle = Atan2Degrees( y, x );

	Vec2 normalizedVec;
	normalizedVec.x = CosDegrees(angle);
	normalizedVec.y = SinDegrees(angle);

	return normalizedVec;
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetReflected( Vec2 const& normalVector ) const
{
	/*
		Solve in N,T coordinates where N is Normal and T is Tangent.
		New velocity has same T but reversed N

		VelN = ProjectOnto(Velocity,  N)
		VelT = Vel - VelN
		newVelocity = VelT - VelN


		Normal is vector from collisionpoint to center normalized
	
	
	*/

	Vec2 ProjectedOntoNormal = GetProjectedOnto2D(*this,normalVector);
	Vec2 VecTangent = *this - ProjectedOntoNormal;
	Vec2 reflectedVec = VecTangent - ProjectedOntoNormal;



	return reflectedVec;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleRadians( float orientationRadians )
{
	float length = GetLength();
	x = length * cosf(orientationRadians);
	y = length * sinf(orientationRadians);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleDegrees( float orientationDegrees )
{
	float length = GetLength();
	x = length * CosDegrees( orientationDegrees );
	y = length * SinDegrees( orientationDegrees );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians( float orientationRadians, float newLength )
{
	x = newLength * cosf(orientationRadians);
	y = newLength * sinf(orientationRadians);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees( float orientationDegrees, float newLength )
{
	x = newLength * CosDegrees(orientationDegrees);
	y = newLength * SinDegrees(orientationDegrees);
}


//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	float radius = GetLength();
	float angle = Atan2Degrees( y, x );
	angle += 90;

	x = radius * CosDegrees(angle);
	y = radius * SinDegrees(angle);
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	float radius = GetLength();
	float angle = Atan2Degrees( y, x );
	angle -= 90;

	x = radius * CosDegrees( angle );
	y = radius * SinDegrees( angle );
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians( float deltaRadians )
{
		float radius = GetLength();
	float angle = atan2f( y, x );
	angle += deltaRadians;

	x = radius * cosf(angle);
	y = radius * sinf(angle);
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees( float deltaDegrees )
{
	float radius = GetLength();
	float angle = Atan2Degrees( y, x );
	angle += deltaDegrees;

	x = radius * CosDegrees( angle );
	y = radius * SinDegrees( angle );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetLength( float newLength )
{
	float angle = atan2f( y, x );
	x = newLength * cosf(angle);
	y = newLength * sinf(angle);
}


//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength( float newLength )
{
	float radius = GetLength();
	float angle = atan2f( y, x );

	Vec2 clampedVec;

	if( newLength < radius )
	{
		x = newLength * cosf( angle );
		y = newLength * sinf( angle );
	}
}


//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float angle = atan2f( y, x );
	x = cosf(angle);
	y = sinf(angle);
}


//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float oldLength = GetLength();
	float angle = atan2f( y, x );
	x = cosf( angle );
	y = sinf( angle );

	return oldLength;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Reflect( Vec2 const& normalVector )
{
	Vec2 ProjectedOntoNormal = GetProjectedOnto2D( *this, normalVector );
	Vec2 VecTangent = *this - ProjectedOntoNormal;
	Vec2 reflectedVec = VecTangent - ProjectedOntoNormal;


	*this = reflectedVec;
	
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetFromText( char const* text )
{
	std::string strText = text;
	
	std::vector<std::string> splitStrings = SplitStringOnDelimeter( text, ',');
	GUARANTEE_OR_DIE(splitStrings.size() == 2, Stringf("Vec2 can't construct from improper string \"%s\"", text));

	x = (float)atof(splitStrings[0].c_str());
	y = (float)atof(splitStrings[1].c_str());

}

bool Vec2::IsAlmostEqual( Vec2 const& other, float epsilon /*= 0.001f */ ) const
{
	return AlmostEqualsFloat( x, other.x, epsilon ) && AlmostEqualsFloat( y, other.y, epsilon );
}
