#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>

class MapRegionType
{
public:
	MapRegionType() = delete;
	MapRegionType( XmlElement const& element );
	~MapRegionType(){}

	static void InitializeMapRegionDefinitions( const XmlElement& rootMapRegionElement );

	static std::map< std::string, MapRegionType*> s_definitions;

protected:
	std::string m_name;
	bool m_isSolid = false;
	//Floor
	//Ceiling
	//Side
};