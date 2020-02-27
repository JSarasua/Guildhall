#pragma once
#include <string>
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
struct Rgba8;
struct Vec2;
struct IntVec2;
struct FloatRange;
struct IntRange;
struct AABB2;

typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;


typedef tinyxml2::XMLDocument XmlDocument;
typedef tinyxml2::XMLError XMLError;

const XMLElement& GetRootElement( XmlDocument& document, const char* xmlFilePath );

std::string ParseXMLAttribute( const XMLElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXMLAttribute( const XMLElement& element, const char* attributeName, const char* defaultValue );
int			ParseXMLAttribute( const XMLElement& element, const char* attributeName, int defaultValue );
char 		ParseXMLAttribute( const XMLElement& element, const char* attributeName, char defaultValue );
bool		ParseXMLAttribute( const XMLElement& element, const char* attributeName, bool defaultValue );
float		ParseXMLAttribute( const XMLElement& element, const char* attributeName, float defaultValue );
Rgba8		ParseXMLAttribute( const XMLElement& element, const char* attributeName, const Rgba8& defaultValue );
Vec2		ParseXMLAttribute( const XMLElement& element, const char* attributeName, const Vec2& defaultValue );
AABB2		ParseXMLAttribute( const XMLElement& element, const char* attributeName, const AABB2& defaultValue );
IntVec2		ParseXMLAttribute( const XMLElement& element, const char* attributeName, const IntVec2& defaultValue );
Strings		ParseXMLAttribute( const XMLElement& element, const char* attributeName, const Strings& defaultValue, const unsigned char delimeter = ',' );
FloatRange	ParseXMLAttribute( const XMLElement& element, const char* attributeName, const FloatRange& defaultValue );
IntRange	ParseXMLAttribute( const XMLElement& element, const char* attributeName, const IntRange& defaultValue );