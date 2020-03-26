#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
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

Rgba8::Rgba8( float const* colorArray )
{
	r = (unsigned char)colorArray[0];
	g = (unsigned char)colorArray[1];
	b = (unsigned char)colorArray[2];
	a = (unsigned char)colorArray[3];
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

void Rgba8::TintColorBy( Rgba8 const& tint )
{
	float currentColor[4];
	float tintColor[4];

	ToFloatArray( currentColor );
	tint.ToFloatArray( tintColor );

	tintColor[0] /= 255.f;
	tintColor[1] /= 255.f;
	tintColor[2] /= 255.f;
	tintColor[3] /= 255.f;

	float tintedColor[4];
	tintedColor[0] = currentColor[0] * tintColor[0];
	tintedColor[1] = currentColor[1] * tintColor[1];
	tintedColor[2] = currentColor[2] * tintColor[2];
	tintedColor[3] = currentColor[3] * tintColor[3];
	Rgba8 newColor = Rgba8(tintedColor);

	*this = newColor;

	return;
}

Rgba8 Rgba8::LerpColorTo( Rgba8 const& startColor, Rgba8 const& endColor, float lerpValue )
{
	float startR	= (float)startColor.r;
	float startG	= (float)startColor.g;
	float startB	= (float)startColor.b;
	float startA	= (float)startColor.a;
	float endR		= (float)endColor.r;
	float endG		= (float)endColor.g;
	float endB		= (float)endColor.b;
	float endA		= (float)endColor.a;

	unsigned char lerpR = (unsigned char)Interpolate( startR, endR, lerpValue );
	unsigned char lerpG = (unsigned char)Interpolate( startG, endG, lerpValue );
	unsigned char lerpB = (unsigned char)Interpolate( startB, endB, lerpValue );
	unsigned char lerpA = (unsigned char)Interpolate( startA, endA, lerpValue );

	Rgba8 color = Rgba8( lerpR, lerpG, lerpB, lerpA );

	return color;
}

void Rgba8::ToFloatArray( float* colorArray ) const
{
	colorArray[0] = (float)r;
	colorArray[1] = (float)g;
	colorArray[2] = (float)b;
	colorArray[3] = (float)a;

	return;
}
