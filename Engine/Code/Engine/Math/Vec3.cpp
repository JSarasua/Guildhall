#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include "Engine/Math/Vec2.hpp"
#include <vector>
#include <string>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec2& copyFrom )
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(0.f)
{}


Vec3::Vec3( const Vec2& copyFrom, float z )
	: x(copyFrom.x),
	y(copyFrom.y),
	z(z)
{

}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3( x * (-1), y * (-1), z * (-1) );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::MakeFromPolarRadians( float orientationRadians, float radius /*= 1.f */ )
{
	Vec3 newVec;
	newVec.x = radius * cosf( orientationRadians );
	newVec.y = radius * sinf( orientationRadians );
	newVec.z = 0.f;
	return newVec;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::MakeFromPolarDegrees( float orientationDegrees, float radius /*= 1.f */ )
{
	Vec3 newVec;
	newVec.x = radius * CosDegrees( orientationDegrees );
	newVec.y = radius * SinDegrees( orientationDegrees );
	newVec.z = 0.f;
	return newVec;
}


Vec3 const Vec3::MakeFromSphericalDegrees( float thetaDegrees, float phiDegrees, float radius /*= 1.f */ )
{
	Vec3 newVec;
	newVec.x = radius * CosDegrees( phiDegrees ) * CosDegrees( thetaDegrees );
	newVec.y = radius * SinDegrees( phiDegrees );
	newVec.z = radius * CosDegrees( phiDegrees ) * SinDegrees( thetaDegrees );
	return newVec;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if( x == compare.x && y == compare.y && z == compare.z ) {
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if( !(x == compare.x && y == compare.y && z == compare.z) ) {
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return sqrtf((x*x) + (y*y) + (z*z));
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	return sqrtf((x*x) + (y*y));
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	return ((x*x) + (y*y) + (z*z));
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
	return ((x*x) + (y*y));
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y,x);
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y,x);
}


//-----------------------------------------------------------------------------------------------
Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	Vec3 rotatedVec;
	float length = GetLengthXY();
	float angle = atan2f(y,x);
	angle += deltaRadians;

	rotatedVec.x = length * cosf(angle);
	rotatedVec.y = length * sinf(angle);
	rotatedVec.z = z;
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	Vec3 rotatedVec;
	float length = GetLengthXY();
	float angle = Atan2Degrees( y, x );
	angle += deltaDegrees;

	rotatedVec.x = length * CosDegrees( angle );
	rotatedVec.y = length * SinDegrees( angle );
	rotatedVec.z = z;
	return rotatedVec;
}


//-----------------------------------------------------------------------------------------------
Vec3 Vec3::GetClamped( float maxLength ) const
{
	float radius = GetLength();
	float angle = atan2f( y, x );

	Vec3 clampedVec;

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
Vec3 Vec3::GetNormalized() const
{
	float length = GetLength();
	Vec3 normalizedVec;

	if( length != 0.f )
	{
		normalizedVec /= length;
	}

	return normalizedVec;
}


//-----------------------------------------------------------------------------------------------
void Vec3::SetFromText( const char* text )
{
	std::string strText = text;

	std::vector<std::string> splitStrings = SplitStringOnDelimeter( text, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 3, Stringf( "Vec3 can't construct from improper string \"%s\"", text ) );

	x = (float)atof( splitStrings[0].c_str() );
	y = (float)atof( splitStrings[1].c_str() );
	z = (float)atof( splitStrings[2].c_str() );
}

void Vec3::Normalize()
{
	float length = GetLength();

	if( length != 0.f )
	{
		*this /= length;
	}
}
