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

const std::vector<std::string> SplitStringOnDelimeter( const char* text, const char delimeter )
{
	std::string textAsString = text;
	std::vector<std::string> delimetedStrings;

	size_t subStringLength = 0;
	size_t subStringStartIndex = 0;
	std::string subString;


	subStringLength = textAsString.find( delimeter , subStringStartIndex);

	while( subStringLength != std::string::npos ) {
		subString = textAsString.substr(subStringStartIndex,subStringLength);
		delimetedStrings.push_back(subString);

		subStringStartIndex = subStringLength + 1;
		subStringLength = textAsString.find( delimeter , subStringStartIndex);
	}

	subString = textAsString.substr(subStringStartIndex,subStringLength);
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



