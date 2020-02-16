#include "Engine/Core/NamedStrings.hpp"

void NamedStrings::PopulateFromXmlElementAttributes( const XMLElement& element )
{
	for( const XMLAttribute* attribute = element.FirstAttribute(); attribute; attribute=attribute->Next()) {
		std::string attributeName = attribute->Name();
		std::string attributeValue = attribute->Value();
		SetValue(attributeName,attributeValue);
	}

}

void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{

	bool value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find(keyName);
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		std::string valueString = keyValueIterator->second;
		value = (bool)atoi(valueString.c_str());
	}


	return value;
}

int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	int value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		std::string valueString = keyValueIterator->second;
		value = atoi( valueString.c_str() );
	}


	return value;
}

float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	float value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		std::string valueString = keyValueIterator->second;
		value = (float)atof( valueString.c_str() );
	}


	return value;
}

std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::string value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		value = keyValueIterator->second;
	}


	return value;
}

std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		value = keyValueIterator->second;
	}


	return value;
}

Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
	Rgba8 value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		value.SetFromText(keyValueIterator->second.c_str());
	}


	return value;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	Vec2 value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		value.SetFromText( keyValueIterator->second.c_str() );
	}


	return value;
}

IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	IntVec2 value = defaultValue;

	MapStringConstIterator keyValueIterator = m_keyValuePairs.find( keyName );
	if( keyValueIterator != m_keyValuePairs.cend() )
	{
		value.SetFromText( keyValueIterator->second.c_str() );
	}


	return value;
}

std::vector<std::string> NamedStrings::GetAllValues() const
{
	std::vector<std::string> allNamedStrings;

	for( MapStringConstIterator keyValueIterator = m_keyValuePairs.begin(); keyValueIterator != m_keyValuePairs.end(); keyValueIterator++ )
	{
		allNamedStrings.push_back(keyValueIterator->second);
	}
	return allNamedStrings;
}
