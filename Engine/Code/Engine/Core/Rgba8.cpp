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
const Rgba8 Rgba8::YELLOW	=	Rgba8(255,255,0,255);
const Rgba8 Rgba8::CYAN		=	Rgba8(0,255,255,255);

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



Rgba8 Rgba8::LerpColorAsHSL( Rgba8 const& startColor, Rgba8 const& endColor, float lerpValue )
{
	float startHSL[4];
	float endHSL[4];
	float newHSL[4];

	Rgba8::RGBToHSL( startColor, startHSL );
	Rgba8::RGBToHSL( endColor, endHSL );

	newHSL[0] = Interpolate( startHSL[0], endHSL[0], lerpValue );
	newHSL[1] = Interpolate( startHSL[1], endHSL[1], lerpValue );
	newHSL[2] = Interpolate( startHSL[2], endHSL[2], lerpValue );
	newHSL[3] = Interpolate( startHSL[3], endHSL[3], lerpValue );

	Rgba8 lerpedColor;
	Rgba8::HSLToRGB( newHSL, lerpedColor );

	return lerpedColor;
}

void Rgba8::RGBToHSL( Rgba8 const& rgba, float* hsl )
{
	float rgbArray[4];
	rgba.ToFloatArray( rgbArray );

	float& r = rgbArray[0];
	float& g = rgbArray[1];
	float& b = rgbArray[2];
	float& a = rgbArray[3];

	r /= 255.f;
	g /= 255.f;
	b /= 255.f;
	a /= 255.f;

	float max = Max( r, Max( g, b ) );
	float min = Min( r, Min( g, b ) );

	float h = 0.f, s = 0.f, l = (max + min) / 2.f;
	if( max == min )
	{
		h = s = 0.f;
	}
	else
	{
		float diff = max - min;
		s = l > 0.5f ? (diff / (2.f - max - min )) : (diff / (max + min));
		
		if( r == max )
		{
			h = ( (g - b) / diff ) + (g < b ? 6.f : 0.f);
		}
		else if( g == max )
		{
			h = ( (b - r) / diff ) + 2.f;
		}
		else if( b == max )
		{
			h = (r - g) / diff + 4.f;
		}
		h /= 6.f;
	}

	hsl[0] = h;
	hsl[1] = s;
	hsl[2] = l;
	hsl[3] = a;

}

void Rgba8::HSLToRGB( float const* hsl, Rgba8& rgba )
{
	float const& h = hsl[0];
	float const& s = hsl[1];
	float const& l = hsl[2];
	float const& a = hsl[3];

	float r, g, b = 0.f;

	if( s == 0.f )
	{
		r = g = b = 1.f;
	}
	else
	{
		float q = l < 0.5f ? (l * (1.f+s)) : (l + s - (l*s));
		float p = (2.f*l) - q;

		r = hue2RGB( p, q, h + 1.f/3.f );
		g = hue2RGB( p, q, h );
		b = hue2RGB( p, q, h - 1.f/3.f );
	}

	r *= 255.f;
	g *= 255.f;
	b *= 255.f;
	float alpha = a * 255.f;

	rgba.r = (unsigned char)r;
	rgba.g = (unsigned char)g;
	rgba.b = (unsigned char)b;
	rgba.a = (unsigned char)alpha;

	return;
}

float Rgba8::hue2RGB( float p, float q, float t )
{
	if( t < 0.f ) t += 1.f;
	if( t > 1.f ) t -= 1.f;
	if( t < 1.f/6.f ) return p + (q - p) * 6.f * t;
	if( t < 1.f/2.f ) return q;
	if( t < 2.f/3.f ) return p + (q - p) * (2.f/3.f - t) * 6.f;
	return p;
}

void Rgba8::ToFloatArray( float* colorArray ) const
{
	colorArray[0] = (float)r;
	colorArray[1] = (float)g;
	colorArray[2] = (float)b;
	colorArray[3] = (float)a;

	return;
}

Vec3 Rgba8::ToVec3() const
{
	Vec3 color;
	color.x = (float)r;
	color.y = (float)g;
	color.z = (float)b;
	color /= 255.f;

	return color;
}

Vec4 Rgba8::ToVec4() const
{
	Vec4 color;
	color.x = (float)r;
	color.y = (float)g;
	color.z = (float)b;
	color.w = (float)a;
	color /= 255.f;

	return color;
}

Rgba8 Rgba8::MakeColorFromVec3( Vec3 const& vecColor )
{
	Rgba8 color;
	color.r = (unsigned char)vecColor.x;
	color.g = (unsigned char)vecColor.y;
	color.b = (unsigned char)vecColor.z;
	color.a = 1;

	return color;
}

Rgba8 Rgba8::MakeColorFromVec4( Vec4 const& vecColor )
{
	Rgba8 color;
	color.r = (unsigned char)vecColor.x;
	color.g = (unsigned char)vecColor.y;
	color.b = (unsigned char)vecColor.z;
	color.a = (unsigned char)vecColor.w;

	return color;
}
