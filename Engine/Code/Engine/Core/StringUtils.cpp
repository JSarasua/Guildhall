#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

const std::string Stringv( const char* format, va_list args )
{
	char buffer[STRINGF_STACK_LOCAL_TEMP_LENGTH];
	vsnprintf_s( buffer, STRINGF_STACK_LOCAL_TEMP_LENGTH, format, args );
	return buffer;
}

const std::vector<std::string> SplitStringOnDelimeter( const char* text, const char delimeter )
{
	std::string textAsString = text;
	std::vector<std::string> delimetedStrings;

	size_t delimeterPosition = 0;
	size_t subStringStartIndex = 0;
	size_t subStringOffset = 0;
	std::string subString;


	delimeterPosition = textAsString.find( delimeter , subStringStartIndex);

	subStringOffset = delimeterPosition;

	while( delimeterPosition != std::string::npos ) {
		subString = textAsString.substr(subStringStartIndex, subStringOffset);

		delimetedStrings.push_back(subString);

		subStringStartIndex = delimeterPosition + 1;
		delimeterPosition = textAsString.find( delimeter , subStringStartIndex);
		//delimeterPosition += 1;
		//subStringLength -= subStringStartIndex;

		subStringOffset = delimeterPosition - subStringStartIndex;
	}

	subString = textAsString.substr(subStringStartIndex,delimeterPosition);
	delimetedStrings.push_back(subString);

	return delimetedStrings;
}

bool SetBoolFromText( const char* text )
{
	std::string textString = text;
	if( textString.compare( "false" ) == 0 )
	{
		return false;
	}
	else if( textString.compare( "true" ) == 0 )
	{
		return true;
	}
	else
	{
		std::string errorMessage = std::string("Failed to convert: ") + text + std::string("to bool");
		ERROR_AND_DIE(errorMessage.c_str());
	}
}

Vec3 GetValueFromString( std::string const& stringValue, Vec3 const& defaultValue )
{
	Vec3 returnVec;
	returnVec.SetFromText( stringValue.c_str() );

	return returnVec;
}

Vec2 GetValueFromString( std::string const& stringValue, Vec2 const& defaultValue )
{
	Vec2 returnVec;
	returnVec.SetFromText( stringValue.c_str() );

	return returnVec;
}

IntVec2 GetValueFromString( std::string const& stringValue, IntVec2 const& defaultValue )
{
	IntVec2 returnVec;
	returnVec.SetFromText( stringValue.c_str() );

	return returnVec;
}

IntRange GetValueFromString( std::string const& stringValue, IntRange const& defaultValue )
{
	IntRange returnRange;
	returnRange.SetFromText( stringValue.c_str() );

	return returnRange;
}

FloatRange GetValueFromString( std::string const& stringValue, FloatRange const& defaultValue )
{
	FloatRange returnRange;
	returnRange.SetFromText( stringValue.c_str() );

	return returnRange;
}

Rgba8 GetValueFromString( std::string const& stringValue, Rgba8 const& defaultValue )
{
	Rgba8 returnColor;
	returnColor.SetFromText( stringValue.c_str() );

	return returnColor;
}

std::string GetValueFromString( std::string const& stringValue, std::string const& )
{
	return stringValue;
}

std::string ToString( Vec3 const& value )
{
	std::string vecStr = Stringf( "%f,%f,%f", value.x, value.y, value.z );

	return vecStr;
}

std::string ToString( Vec2 const& value )
{
	std::string vecStr = Stringf( "%f,%f", value.x, value.y );

	return vecStr;
}

std::string ToString( IntVec2 const& value )
{
	std::string vecStr = Stringf( "%i,%i", value.x, value.y );

	return vecStr;
}

std::string ToString( IntRange const& value )
{
	std::string rangeStr = Stringf( "%i~%i", value.minimum, value.maximum );

	return rangeStr;
}

std::string ToString( FloatRange const& value )
{
	std::string rangeStr = Stringf( "%f~%f", value.minimum, value.maximum );

	return rangeStr;
}

std::string ToString( Rgba8 const& value )
{
	std::string colorStr = Stringf( "%c,%c,%c,%c", value.r, value.g, value.b, value.a );

	return colorStr;
}

std::string ToString( std::string const& value )
{
	return value;
}

