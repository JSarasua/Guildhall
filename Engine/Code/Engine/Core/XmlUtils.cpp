#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"


const XmlElement& GetRootElement( XmlDocument& document, const char* xmlFilePath )
{
	XmlError errorFlag = document.LoadFile( xmlFilePath );
 	std::string errorMessage = "Xml Not Found: ";
 	errorMessage.append(xmlFilePath);
 	GUARANTEE_OR_DIE( errorFlag == XmlError::XML_SUCCESS, errorMessage.c_str() );

	const XmlElement* element = document.RootElement();
	return *element;
}



std::string ParseXMLAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* attribute = element.Attribute(attributeName);
	std::string value = defaultValue;
	if( attribute)
	{
		value = std::string(attribute);
	}
	return value;
}

std::string ParseXMLAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	std::string value = defaultValue;
	if( attribute )
	{
		value = std::string( attribute );
	}
	return value;
}

int ParseXMLAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	int value = defaultValue;
	if( attribute )
	{
		value = atoi( attribute );
	}
	return value;
}

char ParseXMLAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	char value = defaultValue;
	if( attribute )
	{
		value = (char)atoi( attribute );
	}
	return value;
}

bool ParseXMLAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	bool value = defaultValue;
	if( attribute )
	{
		value = SetBoolFromText( attribute );
	}
	return value;
}

float ParseXMLAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	float value = defaultValue;
	if( attribute )
	{
		value = (float)atof( attribute );
	}
	return value;
}

Rgba8 ParseXMLAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	Rgba8 value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

Vec2 ParseXMLAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	Vec2 value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

IntVec2 ParseXMLAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	IntVec2 value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

Strings ParseXMLAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValue, const unsigned char delimeter /*= ',' */ )
{
	const char* attribute = element.Attribute( attributeName );
	Strings value = defaultValue;
	if( attribute )
	{
		value = SplitStringOnDelimeter(attribute, delimeter);
	}
	return value;
}

FloatRange ParseXMLAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	FloatRange value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

AABB2 ParseXMLAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	AABB2 value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

IntRange ParseXMLAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
	const char* attribute = element.Attribute( attributeName );
	IntRange value = defaultValue;
	if( attribute )
	{
		value.SetFromText( attribute );
	}
	return value;
}

