#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <vector>
#include <string>

Vec4::Vec4( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}


Vec4::Vec4( Vec2 const& XY, Vec2 const& ZW )
{
	x = XY.x;
	y = XY.y;
	z = ZW.x;
	w = ZW.y;
}

const bool Vec4::operator==( Vec4 const& b )
{
	if( x == b.x && y == b.y && z == b.z && w == b.w )
	{
		return true;
	}

	return false;
}

const bool Vec4::operator!=( Vec4 const& b )
{
	if( *this == b )
	{
		return false;
	}

	return true;
}

const Vec4 Vec4::operator/( float inverseScale ) const
{
	Vec4 newVec;
	newVec.x = x / inverseScale;
	newVec.y = y / inverseScale;
	newVec.z = z / inverseScale;
	newVec.w = w / inverseScale;

	return newVec;
}

void Vec4::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}

void Vec4::SetFromText( const char* text )
{
	std::string strText = text;

	std::vector<std::string> splitStrings = SplitStringOnDelimeter( text, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 4, Stringf( "Vec4 can't construct from improper string \"%s\"", text ) );

	x = (float)atof( splitStrings[0].c_str() );
	y = (float)atof( splitStrings[1].c_str() );
	z = (float)atof( splitStrings[2].c_str() );
	w = (float)atof( splitStrings[3].c_str() );
}