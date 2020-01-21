#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <string>


IntVec2::IntVec2( const IntVec2& copyFrom ) : 
	x(copyFrom.x),
	y(copyFrom.y)
{}

IntVec2::IntVec2( int initialX, int initialY ) : 
	x(initialX),
	y(initialY)
{}

IntVec2::IntVec2( const Vec2& copyFrom ) :
	x(static_cast<int>(copyFrom.x)),
	y(static_cast<int>(copyFrom.y))
{}

const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2(x + vecToAdd.x, y + vecToAdd.y);
}

const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const IntVec2 IntVec2::operator-() const
{
	return IntVec2( x * (-1), y * (-1) );
}

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( x * uniformScale, y * uniformScale );
}

const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	return IntVec2( x * vecToMultiply.x, y * vecToMultiply.y );

}



void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator*=( const int uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if( !(x == compare.x && y == compare.y) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IntVec2::operator==( const IntVec2& compare ) const
{
	if( x == compare.x && y == compare.y )
	{
		return true;
	}
	else
	{
		return false;
	}
}


int IntVec2::GetTaxicabLength() const
{
	return absInt( x ) + absInt( y );


}

float IntVec2::GetLength() const
{
	Vec2 tempVec(*this);
	return tempVec.GetLength();
}

float IntVec2::GetLengthSquared() const
{
	Vec2 tempVec(*this);
	return tempVec.GetLengthSquared();
}

IntVec2 IntVec2::GetRotated90Degrees() const
{
	//1,0
	//0,1
	//-1,0
	//0,-1
	return IntVec2(-y,x);
}

IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y,-x);
}

float IntVec2::GetOrientationDegrees() const
{
	Vec2 tempVec(*this);
	//tempVec.GetAngleDegrees();
	return tempVec.GetAngleDegrees();
}

float IntVec2::GetOrientationRadians() const
{
	Vec2 tempVec( *this );
	//tempVec.GetAngleRadians();
	return tempVec.GetAngleRadians();
}

void IntVec2::Rotate90Degrees()
{
	int tempX = x;
	x = -y;
	y = tempX;
}

void IntVec2::RotateMinus90Degrees()
{
	int tempX = x;
	x = y;
	y = -tempX;
}

void IntVec2::SetFromText( const char* text )
{
	std::string strText = text;

	std::vector<std::string> splitStrings = SplitStringOnDelimeter( text, ',' );
	GUARANTEE_OR_DIE( splitStrings.size() == 2, Stringf( "IntVec2 can't construct from improper string \"%s\"", text ) );

	x = atoi( splitStrings[0].c_str() );
	y = atoi( splitStrings[1].c_str() );
}

