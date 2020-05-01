#include "Engine/Core/NamedProperties.hpp"


void NamedProperties::SetValue( std::string const& keyName, std::string const& valueName )
{
	SetValue<std::string>( keyName, valueName );
}

std::string NamedProperties::GetValue( std::string const& keyName, std::string const& defaultValue ) const
{
	return GetValue<std::string>( keyName, defaultValue );
}

