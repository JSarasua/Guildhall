#pragma once
#include <map>
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/XmlUtils.hpp"

typedef std::map<std::string, std::string>::const_iterator MapStringConstIterator;

class NamedStrings
{
public:
	NamedStrings() = default;

	void		PopulateFromXmlElementAttributes( const XMLElement& element );
	void		SetValue( const std::string& keyName, const std::string& newValue );

	bool		GetValue( const std::string& keyName, bool defaultValue ) const;
	int			GetValue( const std::string& keyName, int defaultValue ) const;
	float		GetValue( const std::string& keyName, float defaultValue ) const;
	std::string	GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string	GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba8		GetValue( const std::string& keyName, const Rgba8& defaultValue ) const;
	Vec2		GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	IntVec2		GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;

private:
	std::map< std::string, std::string> m_keyValuePairs;
};