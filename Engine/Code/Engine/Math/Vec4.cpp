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