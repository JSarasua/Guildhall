#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <string>

const Rgba8 Rgba8::WHITE	=	Rgba8(255,255,255,255);
const Rgba8 Rgba8::RED		=	Rgba8(255,0,0,255);
const Rgba8 Rgba8::GREEN	=	Rgba8(0,255,0,255);
const Rgba8 Rgba8::BLUE		=	Rgba8(0,0,255,255);
const Rgba8 Rgba8::BLACK	=	Rgba8(0,0,0,255);
const Rgba8 Rgba8::GREY		=	Rgba8(128,128,128,255);

Rgba8::Rgba8 (const Rgba8& copy)
	: r( copy.r )
	, g( copy.g )
	, b( copy.b )
	, a( copy.a )
{
}

Rgba8::Rgba8( unsigned char initialRed, unsigned char initialGreen, unsigned char initialBlue, unsigned char initialAlpha )
	: r( initialRed )
	, g( initialGreen )
	, b( initialBlue )
	, a( initialAlpha )
{
}

bool Rgba8::operator==( const Rgba8& compare ) const
{
	if( r == compare.r &&
		g == compare.g &&
		b == compare.b &&
		a == compare.a )
	{
		return true;
	}
	else
	{
		return false;
	}

}

void Rgba8::operator=( const Rgba8& copyFrom )
{
	r = copyFrom.r;
	g = copyFrom.g;
	b = copyFrom.b;
	a = copyFrom.a;
}

const Rgba8 Rgba8::operator+( const Rgba8& copyRgba8 ) const
{
	Rgba8 newRgba8;

	newRgba8.r = r+copyRgba8.r;
	newRgba8.g = g+copyRgba8.g;
	newRgba8.b = b+copyRgba8.b;
	newRgba8.a = a+copyRgba8.a;

	return newRgba8;
}

void Rgba8::SetFromText( const char* text )
{
	std::string strText = text;

	std::vector<std::string> splitStrings = SplitStringOnDelimeter( text, ',' );

	if( splitStrings.size() == 3 )
	{
		r = (unsigned char)atoi( splitStrings[0].c_str() );
		g = (unsigned char)atoi( splitStrings[1].c_str() );
		b = (unsigned char)atoi( splitStrings[2].c_str() );
		a = 255;
	}
	else if( splitStrings.size() == 4 )
	{
		r = (unsigned char)atoi( splitStrings[0].c_str() );
		g = (unsigned char)atoi( splitStrings[1].c_str() );
		b = (unsigned char)atoi( splitStrings[2].c_str() );
		a = (unsigned char)atoi( splitStrings[3].c_str() );
	}
	else
	{
		GUARANTEE_OR_DIE( splitStrings.size() == 3 || splitStrings.size() == 4, Stringf( "Vec2 can't construct from improper string \"%s\"", text ) );
	}



}
